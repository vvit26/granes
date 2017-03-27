#include "../../granes_lib/src/granes.h"

#include <stdexcept>
#include <iostream>


// switch off output to console in Release mode
class Cons {
    template<typename T>
    friend Cons & operator << (Cons & cons, T const & t) {
#ifdef _DEBUG
        std::cout << t;
#endif
        return cons;
    }
} cons;


int main(int argc, char const * argv[]) {
    try {
        grd::Processor processor;

        switch (processor.Process(argc, argv)) {
            // later, we may have different return values. like missing input file and then ask the user or use the previously used name
            case grd::PrResult::_args:
                cons << "extra command line arguments, using only the first argument\n";
                processor.Process(2, argv);
                break;
            case grd::PrResult::_ok:
                cons << "Finished\n";
                break;
        }
        return 0;
    }
    // errors are reported as exceptions
    catch (std::exception & e) {
        cons << e.what() << '\n';
        return 1;
    }
}


