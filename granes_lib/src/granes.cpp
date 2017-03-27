#include "granes.h"

#include <stdexcept>
#include <algorithm>
#include <locale>
#include <cctype>


using namespace std::string_literals;

namespace grd {

    // the output file name will have this suffix
    char const _suffix[] = "-graded.txt";

    namespace detail {

        // read one record
        std::istream & operator >> (std::istream & in, PersonGrade & pg) {
            in >> pg.first_name >> pg.second_name >> pg.score;
            return in;
        }

        // write one record
        std::ostream & operator << (std::ostream & out, PersonGrade const & pg) {
            out << pg.second_name << ", " << pg.first_name << ", " << pg.score << '\n';
            return out;
        }

        // read a sequence of records
        std::istream & operator >> (std::istream & in, Data & sq) {
            if (!in.eof()) {
                PersonGrade pg;
                for (;;) {
                    in >> pg;
                    if (!in.eof())
                        sq.push_back(std::move(pg));
                    else
                        break;
                }
            }
            return in;
        }

        // write a sequence of records
        std::ostream & operator << (std::ostream & out, Data const & sq) {
            for (auto & pg : sq) {
                out << pg;
            }
            return out;
        }

        void Model::Sort(Data & sq) {
            // an alternative is comparison with e.g. std::lexicographical_compare
            // it would be simpler for developer and less prone to errors but slower as whould be run twice
            auto compare = [](std::string const & l, std::string const & r) -> int {
                for (size_t i = 0, sz = std::min(l.size(), r.size()); i < sz; ++i) {
                    auto lc = std::tolower(l[i]), rc = std::tolower(r[i]);
                    if (lc < rc)
                        return -1;
                    else if (lc > rc)
                        return 1;
                }
                return l.size() < r.size() ? -1 : l.size() > r.size();
            };
            // in the sort function, specify the sorting criterion
            std::sort(sq.begin(), sq.end(), [&compare](PersonGrade const & l, PersonGrade const & r) -> bool {
                if (l.score > r.score)
                    return true;
                else if (l.score < r.score)
                    return false;

                auto res = compare(l.second_name, r.second_name);
                if (res < 0)
                    return true;
                else if (res > 0)
                    return false;
                else
                    return compare(l.first_name, r.first_name) < 0;
            });
        }

        View::View(std::string const & fname) : in(fname) {
            if (!in)
                throw std::runtime_error("could not open file "s + fname);

            out.open(fname + _suffix/* add "-graded.txt" to the name */);
            if (!out)
                throw std::runtime_error("could not open file "s + fname + _suffix/* add "-graded.txt" to the name */);

            // This ctype facet classifies commas as whitespace
            struct csv_whitespace : std::ctype<char> {
                csv_whitespace() : ctype(get_table()) {}
                static const mask * get_table() {
                    // make a copy of the "C" locale table
                    static std::vector<mask> v(classic_table(), classic_table() + table_size);
                    v[','] |= std::ctype_base::space;  // comma will be classified as whitespace
                    return &v[0];
                }
            };
            in.imbue(std::locale(in.getloc(), new csv_whitespace()/* this allocation is not leaked */));
        }

    }

    using namespace detail;

    // open the files, read-write, call Data::Process() to process
    PrResult Processor::Process(int argc, char const * argv[]/* same as in main */) {
        if (argc == 1)
            throw std::runtime_error("input file not specified");

        else if (argc > 2)
            return PrResult::_args;

        Data        data;
        View        view(argv[1]);
        Model       model;
        Controller  controller(&model, &view, &data);

        // read the records
        controller.Input();

        // sort the records
        controller.Sort();

        // write the sorted records
        controller.Output();

        return PrResult::_ok;
    }
}

