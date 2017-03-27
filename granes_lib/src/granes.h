#ifndef __Grades_H__
#define __Grades_H__


#include <iostream>
#include <string>
#include <cstdint>
#include <vector>
#include <fstream>



namespace grd {

    // result of Processor::Process()
    enum PrResult {
        _ok, _args
    };

    class Processor {

    public:

        // first two arguments are the same as in function main, advantage - can test the whole process
        // could be made static
        PrResult Process(int argc/* same as in main */, char const * argv[]/* same as in main */);

    };

    // this namespace is used in tests, but not in function main
    namespace detail {

        struct PersonGrade {
            // the compiler will create a move constructor
            //PersonGrade(PersonGrade &&) noexcept; This is used when growing the vector and reading sequence

            uint16_t        score;
            std::string     first_name;
            std::string     second_name;
        };

        using Data = std::vector<PersonGrade>;

        // read from input stream in
        std::istream & operator >> (std::istream & in, Data & sq);

        // write to output stream out
        std::ostream & operator <<(std::ostream & out, Data const & sq);


        class IModel {

        public:

            virtual void Sort(Data &) = 0;
        };


        class Model : public IModel {

        public:

            // sort the records
            // could be made static
            virtual void Sort(Data & sq) override;  // may be made virtual if polymorfic behavior with different models is needed
        };


        class IView {

        public:

            virtual void Input(Data & sq) = 0;
            virtual void Output(Data const & sq) = 0;
        };


        class View : public IView {

        public:

            View(std::string const & fname);
            virtual void Input(Data & sq) override { in >> sq; }  // may be made virtual if polymorfic behavior with different Views is needed
            virtual void Output(Data const & sq) override { out << sq; }  // may be made virtual if polymorfic behavior with different Views is needed


        private:

            std::ifstream   in;
            std::ofstream   out;
        };


        class Controller {

        public:

            Controller(IModel * model, IView * view, Data * data) {
                this->model = model;
                this->view = view;
                this->data = data;
            }
            void Input() { view->Input(*data); }
            void Sort() { model->Sort(*data); }
            void Output() { view->Output(*data); }


        private:

            IModel *    model;
            IView *     view;
            Data *      data;
        };


    }  // namespace detail

}  // namespace grd


#endif

