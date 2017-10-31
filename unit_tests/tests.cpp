//here i run all my tests
//tests.cpp, it is a cheat, nothing wrong
//include the file that i want to test (where my functions are in to)
#include "string_utils.c"
#include "trie_node.c"
#include"trie.c"
#include <gtest/gtest.h>
 
TEST(CaseA, BigLine){
    line_manager lm;
    FILE* in;
    in=stdin;
    line_manager_init(&lm, in, 'I');
    printf("please give a really big sentence\n");
    ASSERT_EQ(true,lm_fetch_line(&lm));
    line_manager_fin(&lm);
}

/*Asked for invalid task px delete as DELETE*/
/*TEST(CaseB, InvalidTask){
    line_manager lm;
    FILE* in;
    in=stdin;
    printf("please give a query which is not started by A/Q/D, as DELETE bla bla \n");
    line_manager_init(&lm, in, 'Q');
    ASSERT_EQ(false,lm_fetch_line(&lm));
    line_manager_fin(&lm);
    fclose(in);
}*/

/*TEST(CaseC, EmptyQuery){
    line_manager lm;
    FILE* in;
    in=stdin;
    printf("please give a query which starts by A/Q/D and nothing more \n");
    line_manager_init(&lm, in, 'Q');
    ASSERT_EQ(false, lm_fetch_line(&lm));
    line_manager_fin(&lm);
    fclose(in);
}*/

/*TEST(CaseC, EmptyNgrams){
    line_manager lm;
    FILE* in;
    in=stdin;
    printf("please give a empty ngram(multiple spaces) \n");
    line_manager_init(&lm, in, 'I');
    ASSERT_EQ(false, lm_fetch_line(&lm));
    line_manager_fin(&lm);
    fclose(in);
}*/

TEST(DeleteNgram, SearchAfter){

}

TEST(DeleteNgramThatexistsMultipleTimes, SearchAfter){

}

TEST(AddDeleteSearch, TheSameNgram){
    
}
/*

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

*/
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

