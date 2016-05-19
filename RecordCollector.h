
#ifndef RECORDCOLLECTOR_H
#define RECORDCOLLECTOR_H


#include "PriceRecord.h"
#include <set>



class RecordCollector
{
public:
    void InsertRecord( const PriceRecord& pr ) {
        records_.insert( pr );
    }

    void WriteRecords()
    {
        std::string lastSym = "";
        uint64_t count = 0;
        FILE* file = NULL;

        for( const auto& rec : records_ )
        {
            const std::string sym = rec.GetSymbol();
            if( sym != lastSym )	// close the old file & open a new one
            {
                if( file )
                {
                    fprintf( stdout, "%s Rows: %d\n", lastSym.c_str(), count );
                    fclose( file );
                    file = NULL;
                    count = 0;
                }
                std::string fname = sym + ".csv";
                file = fopen( fname.c_str(), "w" );
                lastSym = sym;
            }

            ++count;
            fprintf( file, "%.2lf,%d,%d\n", rec.GetPrice(), rec.GetQuantity(), rec.GetSequence() );
        }

        // don't forget to write the last symbol and its count
        if( file )
        {
            fprintf( stdout, "%s Rows: %d\n", lastSym.c_str(), count );
            fclose( file );
            file = NULL;
        }
    }

private:
    std::set< PriceRecord > records_;
};


#endif /* RECORDCOLLECTOR_H */

