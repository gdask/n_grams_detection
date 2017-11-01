//here i run all my tests
//tests.cpp, it is a cheat, nothing wrong
//include the file that i want to test (where my functions are in to)
#include "string_utils.c"
#include "trie_node.c"
#include"trie.c"
#include <gtest/gtest.h>
#include <string>
 
/*
TEST(CaseA, BigLine){
    line_manager lm;
    FILE* in;
    in=fopen("input/BigLine", "r");
    line_manager_init(&lm, in, 'I');
    ASSERT_EQ(true, lm_fetch_line(&lm));
    line_manager_fin(&lm);
    fclose(in);
}*/

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
    line_manager lm;
    FILE* in, *out;
    in=fopen("input/DeleteNgramTest", "r");
    out=fopen("output/DeleteNgramTest", "w+");
    line_manager_init(&lm, in, 'Q');
    trie db;
    trie_init(&db,5);
    result_manager rm;
    result_manager_init(&rm, out);

    //add
    lm_fetch_line(&lm);
    trie_insert(&db, &lm);

    //delete
    lm_fetch_line(&lm);
    trie_delete(&db, &lm);

    //Search
    lm_fetch_line(&lm);
    trie_search(&db, &lm, &rm);

    char buffer[100];
    int size=100;
    fgets(buffer, size, out);
    EXPECT_EQ("-1\n", buffer);

    trie_fin(&db);
    result_manager_fin(&rm);
    line_manager_fin(&lm);
    fclose(in);
    fclose(out);

}

/*
TEST(DeleteNgramThatexistsMultipleTimes, SearchAfter){

}

TEST(AddDeleteSearch, TheSameNgram){

}*/
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

