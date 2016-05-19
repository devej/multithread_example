
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>
#include <set>
#include <string>
#include "PriceRecord.h"
#include "RecordCollector.h"
#include <mutex>
#include <chrono>
#include <condition_variable>



class Consumer
{
public:
    Consumer() : finished_(false) {}
    Consumer( const Consumer& ) = delete;
    const Consumer& operator=( const Consumer& ) = delete;

    void AddRecord( const PriceRecord& rec )
    {
        std::unique_lock< std::mutex > guard( mtx_ );
        records_.InsertRecord( rec );
    }

    void SetFinished() { finished_ = true; cnd_.notify_one(); }

    void operator() ()
    {
        std::unique_lock< std::mutex > guard( mtx_ );
        cnd_.wait( guard, [this]{return finished_;} );   // use predicate form to protect against spurious wake-up calls
        records_.WriteRecords();
        //std::cout << "Consumer finished\n";
    }

private:
    std::mutex              mtx_;
    std::condition_variable cnd_;
    bool                    finished_;
    RecordCollector         records_;
};


class Producer
{
public:
    Producer( const std::string& filename, Consumer& consumer ) : filename_(filename), consumer_(consumer)
    {
        // Open the file, read it line by line, sending each line as-is.
        FILE* file = fopen( filename_.c_str(), "r" );
        if( !file ) {
            std::cerr << "Unable to open " << filename_ << "\n";
            throw std::logic_error( "Couldn't open file" );
        }

        char readBuffer[ 1024 ] = {0};
        uint64_t recordsRead = 0;

        while( fgets( readBuffer, sizeof(readBuffer), file ) ) {
            records_.push_back( readBuffer );
        }

        fclose( file );
    }

    Producer( const Producer& ) = delete;
    Producer& operator=( const Producer& ) = delete;

    void operator() ()
    {
        //std::cout << "Producer read " << records_.size() << " records from " << filename_ << "\n";
        for( const auto& r : records_ ) {
            //std::cout << r;
            consumer_.AddRecord( MakePriceRecord(r) );
        }
        consumer_.SetFinished();
    }

private:
    std::string                 filename_;
    std::vector< std::string >  records_;
    Consumer&                   consumer_;

    void Split( const std::string &s, char delim, std::vector< std::string >& elems )
    {
        std::stringstream ss(s);
        std::string item;
        while( std::getline(ss, item, delim) )
        {
            elems.push_back( item );
        }
    }

    PriceRecord MakePriceRecord( const std::string& data )
    {
        std::vector< std::string > elements;
        Split( data, ',', elements );
        if( 4 != elements.size() ) {
            std::cerr << "Expected 4 elements, got " << elements.size() << " :" << data;
            throw std::logic_error( "Wrong number of elements" );
        }

        // sample data
        //0x000003ef,MSFT,9.52,3700
        //0x000003f0,AAPL,4.6,2300

        //const unsigned long seq = std::stoul( elements[0], nullptr, 16 );
        //const std::string sym = elements[1];
        //const double price = std::stod( elements[2], nullptr );
        //const unsigned long qty = std::stoul( elements[3], nullptr, 10 );

        return {
            std::stoul( elements[3], nullptr, 10 ),
            std::stoul( elements[0], nullptr, 16 ),
            std::stod( elements[2], nullptr ),
            elements[1] };
    }
};



int main( int argc, char**argv )
{
   if( argc != 2 ) {
        fprintf( stderr, "Too many arguments.  Just provide a filename please.\n" );
        return 1;
    }

    Consumer c;
    Producer p( argv[1], c );
    
    std::thread producer_thread( std::ref(p) );
    std::thread consumer_thread( std::ref(c) );
    consumer_thread.join();
    producer_thread.join();

    return 0;
}
