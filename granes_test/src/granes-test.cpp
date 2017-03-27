#include "../../granes_lib/src/granes.h"

// we use Google Test library
#include <gtest/gtest.h>

#include <sstream>


using namespace std::string_literals;


TEST(Grades, NoInput) {
    char const * argv[] = { "grade-scores.exe" };
    grd::Processor processor;
    ASSERT_THROW(processor.Process(1, argv), std::runtime_error);
}

TEST(Grades, ExtraInput) {
    char const * argv[] = { "grade-scores.exe" };
    grd::Processor processor;
    ASSERT_EQ(grd::PrResult::_args, processor.Process(3, argv));
}

TEST(Grades, WrongInput) {
    char const * argv[] = { "grade-scores.exe", "not_esisting.txt" };
    grd::Processor processor;
    // an exception must have been thrown
    ASSERT_THROW(processor.Process(2, argv), std::runtime_error);
}

TEST(Grades, FileName) {
    // create input file
    {
        std::ofstream{ "test-input.txt" };
    }
    // this will create output file
    {
        grd::detail::View view("test-input.txt");
    }
    // check the output file is created
    std::ifstream in("test-input.txt-graded.txt");
    ASSERT_TRUE(in.operator bool());
}

TEST(Grades, CorrectEmptyInput) {
    {
        std::ofstream{ "input.txt" };
    }
    char const * argv[] = { "grade-scores.exe", "input.txt" };
    grd::Processor processor;
    ASSERT_EQ(grd::PrResult::_ok, processor.Process(2, argv));
}

// create a file with the given content, run the algorithm and return the result
std::string GetOutput(std::string const & ins) {
    // create the input file
    {
        std::ofstream out("test.txt");
        out << ins;
    }

    grd::Processor processor;
    char const * argv[] = { "grade-scores.exe", "test.txt" };
    processor.Process(2, argv);

    std::ifstream in("test.txt-graded.txt");
    std::stringstream stream;
    stream << in.rdbuf();

    return stream.str();
}

TEST(Grades, CorrectAgeSurname) {
    ASSERT_EQ(R"(BUNDY, TED, 88
sMITH, ALLAN, 85
SMITH, FRANCIS, 85
KING, MADISON, 83
)",
GetOutput(R"(TED, BUNDY, 88
ALLAN, sMITH , 85
MADISON , KING, 83
FRANCIS, SMITH, 85
)"));
}

TEST(Grades, CorrectAgeSurnameName) {
    ASSERT_EQ(R"(BUNDY, ALISON, 88
BUNDY, TERESSA, 88
KING, MADISON, 88
kings, Alan, 88
SMITH, FRANCIS, 85
SMITH, ALLAN, 70
)",
GetOutput(R"(TERESSA, BUNDY, 88
ALISON, BUNDY, 88
ALLAN, SMITH, 70
MADISON, KING, 88
Alan, kings, 88
FRANCIS, SMITH, 85
)"));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
