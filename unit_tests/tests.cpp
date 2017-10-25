//here i run all my tests
//tests.cpp, it is a cheat, nothing wrong
//include the file that i want to test (where my functions are in to)
#include "string_utils.c"
#include "trie_node.c"
#include"trie.c"
#include <gtest/gtest.h>
 

TEST(CompletePhrase, IsANotphrase){
    ASSERT_EQ(false, complete_phrase("this is a"));
}

TEST(CompletePhrase, IsAphrase){
    ASSERT_EQ(true, complete_phrase("this is a\n"));
}

TEST(Fetchline, ALLGOOD){
    line_manager lm;
    FILE* in;
    in=fopen("commands.txt","r");
    line_manager_init(&lm, in, 'I');
    ASSERT_EQ(true,lm_fetch_line(&lm));
    line_manager_fin(&lm);
    fclose(in);
}

TEST(Fetchline, WrongInput){
    line_manager lm;
    FILE* in;
    in=fopen("commands.txt","r");
    line_manager_init(&lm, in, 'Q');
    ASSERT_EQ(false ,lm_fetch_line(&lm));
    line_manager_fin(&lm);
    fclose(in);
}

TEST(Fetchline, Unitisializedbuf){
    line_manager lm;
    FILE* in;
    in=fopen("commands.txt","r");
    //line_manager_init(&lm, in, 'Q');
    //expected code is -1 but in our system -1 is 255
    ASSERT_EXIT(lm_fetch_line(&lm), ::testing::ExitedWithCode(255),"Object is not initisialised:: lm_fetch_line\n");
    line_manager_fin(&lm);
    fclose(in);
}

TEST(FetchNgram, ALLGOOD){
    line_manager lm;
    FILE* in;
    in=fopen("test.work","r");
    line_manager_init(&lm, in, 'Q');
    lm_fetch_line(&lm);
    ASSERT_EQ(true ,lm_fetch_ngram(&lm));
    line_manager_fin(&lm);
    //printf("allgood");
    fclose(in);
}

TEST(FetchNgram, NoLine){
    line_manager lm;
    FILE* in;
    in=fopen("test.work","r");
    line_manager_init(&lm, in, 'Q');
    //lm_fetch_line(&lm);
    ASSERT_EQ(false ,lm_fetch_ngram(&lm));
    line_manager_fin(&lm);
    fclose(in);
}

TEST(ISQUERY, YES){
    line_manager lm;
    FILE* in;
    in=fopen("test.work","r");
    line_manager_init(&lm, in, 'Q');
    lm_fetch_line(&lm);
    lm_fetch_ngram(&lm);
    ASSERT_EQ(true ,lm_is_query(&lm));
    line_manager_fin(&lm);
    fclose(in);
}

TEST(ISQUERY, NO){
    line_manager lm;
    FILE* in;
    in=fopen("test.work","r");
    line_manager_init(&lm, in, 'Q');
    lm_fetch_line(&lm);
    lm_fetch_line(&lm);
    lm_fetch_line(&lm);
    ASSERT_EQ(false ,lm_is_query(&lm));
    line_manager_fin(&lm);
    fclose(in);
}

/*GEORGE*/
/*TEST(InsertToTree, Allgood){
    line_manager lm;
    FILE* in;
    in=fopen("commands.txt","r");
    do the routine for one insertion
    change delete to return true-false and check if true is returned
    ASSERT_EQ(true, insert(....));
}*/

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

