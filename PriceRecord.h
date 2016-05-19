//
//  PriceRecord
//
#pragma once

#include <string>
#include <ostream>
#include <iomanip>


class PriceRecord
{
public:

    PriceRecord( uint64_t Qty, uint64_t SeqNum, double Price, const std::string& Symbol ) :
        qty_( Qty ), seq_( SeqNum ), price_( Price ), sym_( Symbol )
    { }

    ~PriceRecord() { }

    bool operator < ( const PriceRecord& rhs ) const
    {
        // sort by sym, price, seq
        if( sym_ != rhs.sym_ )
            return sym_ < rhs.sym_;

        if( price_ != rhs.price_ )
            return price_ < rhs.price_;

        return seq_ > rhs.seq_;
    }

    // individual getters
    std::string     GetSymbol()     const   { return sym_; }
    uint64_t        GetQuantity()   const   { return qty_; }
    uint64_t        GetSequence()   const   { return seq_; }
    double          GetPrice()      const   { return price_; }

private:
    uint64_t       qty_;
    uint64_t       seq_;
    double         price_;
    std::string    sym_;
};	// PriceRecord

//
// Just another way to print a PriceRecord
//
std::ostream& operator<<( std::ostream& oss, const PriceRecord& rec )
{
    //fprintf( file, "%.2lf,%d,%d\n", rec.GetPrice(), rec.GetQuantity(), rec.GetSequence() );
    return oss << std::fixed << std::setprecision(2) << rec.GetPrice() << "," << rec.GetQuantity() << "," << rec.GetSequence() << "\n";
}

