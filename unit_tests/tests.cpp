//here i run all my tests
//tests.cpp, it is a cheat, nothing wrong
//include the file that i want to test (where my functions are in to)
#include "string_utils.c"
#include "trie_node.c"
#include"trie.c"
#include <gtest/gtest.h>
#include <string>
 

TEST(Buffersize, BigLine){
    line_manager lm;
    FILE* in;
    in=fopen("input/BigLine", "r");
    line_manager_init(&lm, in, 'I');
    EXPECT_EQ(true, lm_fetch_line(&lm));
    line_manager_fin(&lm);
    fclose(in);
}

/*Asked for invalid task px delete as DELETE*/
TEST(Errorhandling, InvalidTask){
    line_manager lm;
    FILE* in;
    in=fopen("input/InvalidTaskFile","r");
    line_manager_init(&lm, in, 'Q');
    bool has_line;
    has_line=lm_fetch_line(&lm);
    while(has_line==true){
        if(lm_is_insert(&lm)==true||lm_is_delete(&lm)==true||lm_is_query(&lm)==true){
            EXPECT_EQ(true, has_line);
        }
        else{
            EXPECT_EQ(false, has_line);
        }
        has_line=lm_fetch_line(&lm);
    }
    line_manager_fin(&lm);
    fclose(in);
}

TEST(Emptyline, EmptyQuery){
    line_manager lm;
    FILE* in;
    in=fopen("input/EmptyQuery", "r");
    line_manager_init(&lm, in, 'Q');
    EXPECT_EQ(false, lm_fetch_line(&lm));
    line_manager_fin(&lm);
    fclose(in);
}

TEST(Emptyline, EmptyNgrams){
    line_manager lm;
    FILE* in;
    in=fopen("input/EmptyNgram","r");
    line_manager_init(&lm, in, 'I');
    EXPECT_EQ(false, lm_fetch_line(&lm));
    line_manager_fin(&lm);
    fclose(in);
}

TEST(Spaces, MultipleSpaces){
    line_manager lm;
    FILE* in, *out;
    in=fopen("input/MultipleSpaces", "r");
    out=fopen("output/MultipleSpaces", "w+");
    line_manager_init(&lm, in, 'Q');
    trie db;
    trie_init(&db,5);
    result_manager rm;
    result_manager_init(&rm, out);
    bool has_line;
    has_line=lm_fetch_line(&lm);
    while(has_line==true){
        if(lm_is_insert(&lm)==true){
            lm_fetch_ngram(&lm);
            trie_insert(&db,&lm);
        }
        else if(lm_is_delete(&lm)==true){
            lm_fetch_ngram(&lm);
            trie_delete(&db,&lm);
        }
        else if(lm_is_query(&lm)==true){
            trie_search(&db,&lm,&rm);
        }
        else{
            fprintf(stderr,"Corrupted line\n");
        }
        has_line=lm_fetch_line(&lm);
    }

    char buffer[100];
    int size=100;
    fseek(out, 0, SEEK_SET);
    if(fgets(buffer, size, out)){
        int r;
        r=strcmp("s s|s s s\n", buffer);
        EXPECT_EQ(0, r);
    }

    trie_fin(&db);
    result_manager_fin(&rm);
    line_manager_fin(&lm);
    fclose(in);
    fclose(out);
}

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
    bool has_line;
    has_line=lm_fetch_line(&lm);
    while(has_line==true){
        if(lm_is_insert(&lm)==true){
            lm_fetch_ngram(&lm);
            trie_insert(&db,&lm);
        }
        else if(lm_is_delete(&lm)==true){
            lm_fetch_ngram(&lm);
            trie_delete(&db,&lm);
        }
        else if(lm_is_query(&lm)==true){
            trie_search(&db,&lm,&rm);
        }
        else{
            fprintf(stderr,"Corrupted line\n");
        }
        has_line=lm_fetch_line(&lm);
    }

    char buffer[100];
    int size=100;
    fseek(out, 0, SEEK_SET);
    if(fgets(buffer, size, out)){
        int r;
        r=strcmp("-1\n", buffer);
        EXPECT_EQ(0, r);
    }

    trie_fin(&db);
    result_manager_fin(&rm);
    line_manager_fin(&lm);
    fclose(in);
    fclose(out);

}


TEST(DeleteNgram, ExistMultipleTimes){
    line_manager lm;
    FILE* in, *out;
    in=fopen("input/ExistMultipleTimes", "r");
    out=fopen("output/ExistMultipleTimes", "w+");
    line_manager_init(&lm, in, 'Q');
    trie db;
    trie_init(&db,5);
    result_manager rm;
    result_manager_init(&rm, out);
    bool has_line;
    has_line=lm_fetch_line(&lm);
    while(has_line==true){
        if(lm_is_insert(&lm)==true){
            lm_fetch_ngram(&lm);
            trie_insert(&db,&lm);
        }
        else if(lm_is_delete(&lm)==true){
            lm_fetch_ngram(&lm);
            trie_delete(&db,&lm);
        }
        else if(lm_is_query(&lm)==true){
            trie_search(&db,&lm,&rm);
        }
        else{
            fprintf(stderr,"Corrupted line\n");
        }
        has_line=lm_fetch_line(&lm);
    }

    char buffer[100];
    int size=100;
    fseek(out, 0, SEEK_SET);
    if(fgets(buffer, size, out)){
        int r;
        r=strcmp("this is a dog\n", buffer);
        EXPECT_EQ(0, r);
    }

    trie_fin(&db);
    result_manager_fin(&rm);
    line_manager_fin(&lm);
    fclose(in);
    fclose(out);
}


TEST(AddDeleteSearch, TheSameNgram){
    line_manager lm;
    FILE* in, *out;
    in=fopen("input/TheSameNgram", "r");
    out=fopen("output/TheSameNgram", "w+");
    line_manager_init(&lm, in, 'Q');
    trie db;
    trie_init(&db,5);
    result_manager rm;
    result_manager_init(&rm, out);
    bool has_line;
    has_line=lm_fetch_line(&lm);
    while(has_line==true){
        if(lm_is_insert(&lm)==true){
            lm_fetch_ngram(&lm);
            trie_insert(&db,&lm);
        }
        else if(lm_is_delete(&lm)==true){
            lm_fetch_ngram(&lm);
            trie_delete(&db,&lm);
        }
        else if(lm_is_query(&lm)==true){
            trie_search(&db,&lm,&rm);
        }
        else{
            fprintf(stderr,"Corrupted line\n");
        }
        has_line=lm_fetch_line(&lm);
    }

    char buffer[100];
    int size=100;
    fseek(out, 0, SEEK_SET);
    if(fgets(buffer, size, out)){
        int r;
        r=strcmp("-1\n", buffer);
        EXPECT_EQ(0, r);
    }

    trie_fin(&db);
    result_manager_fin(&rm);
    line_manager_fin(&lm);
    fclose(in);
    fclose(out);
}

/*

TEST(CompletePhrase, IsANotphrase){
    EXPECT_EQ(false, complete_phrase("this is a"));
}

TEST(CompletePhrase, IsAphrase){
    EXPECT_EQ(true, complete_phrase("this is a\n"));
}

TEST(Fetchline, ALLGOOD){
    line_manager lm;
    FILE* in;
    in=fopen("commands.txt","r");
    line_manager_init(&lm, in, 'I');
    EXPECT_EQ(true,lm_fetch_line(&lm));
    line_manager_fin(&lm);
    fclose(in);
}

TEST(Fetchline, WrongInput){
    line_manager lm;
    FILE* in;
    in=fopen("commands.txt","r");
    line_manager_init(&lm, in, 'Q');
    EXPECT_EQ(false ,lm_fetch_line(&lm));
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
    EXPECT_EQ(true, insert(....));
}*/

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

