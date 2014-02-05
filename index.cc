/* quickstartindex.cc: Simplest possible indexer
 *
 * ----START-LICENCE----
 * Copyright 1999,2000,2001 BrightStation PLC
 * Copyright 2003,2004 Olly Betts
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301
 * USA
 */

#include <xapian.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <set>
#include <dirent.h> // directory header

using namespace std;
struct stat sb;

set<int> generateTerms(char *filepath) {
  int x;
  set<int> result;
  FILE *fp = fopen(filepath, "r");
  while( fscanf(fp, "%d", &x) != EOF )
    result.insert(x);
  return result;
}

multiset<int> generateTerms2(char *filepath) {
  int x;
  multiset<int> result;
  FILE *fp = fopen(filepath, "r");
  while( fscanf(fp, "%d", &x) != EOF )
    result.insert(x);
  return result;
}

char OpenFile(char *contentName, char *idName, char *timeName, char *databaseName){
    	FILE *fp_content;
    	FILE *fp_id;
    	FILE *fp_time;

    	char *doc;
    	char *id;
    	char *time;

    	int digit;
    	char Doc[50];
    	char Id[50];
    	char Time[20];
    	char temp_Doc[100000];
    	char temp_Id[50];
    	int i = 0;
    	int stat_key = 1;
    	int temp_no;
    	char ic[50]; //int to char
    	char x[50];

	set <int>::iterator si;
	if (((fp_content = fopen(contentName, "r"))== NULL)){ //check whether the file exists or not
		printf("File not exists!!!\n");
    		return -1;
	}
	else{
		fp_id = fopen(idName, "r");
		fp_time = fopen(timeName, "r");

	// Make the database
        Xapian::WritableDatabase database(databaseName, Xapian::DB_CREATE_OR_OPEN);

	// Make the document
	Xapian::Document newdocument;

        // Generate the content for terms
	multiset<int> doc_terms = generateTerms2(contentName);

        // Put the time in the document
	fscanf (fp_time, "%s ", Time);
	newdocument.add_value(4, Time);

	// Put the terms into the document
        int t_id = 0;
	for (si=doc_terms.begin(); si!=doc_terms.end();si++){
            sprintf(ic, "%d", *si);
            newdocument.add_term(ic);
            t_id++;
        }

	// Put the id and version
	temp_Id[0]='\0';
	while(fscanf (fp_id, "%s", Id) != EOF){
            strcat(temp_Id, Id); //combine the words
            strcat(temp_Id, " ");
        }

        // add doc_Id and ver_id into document in slot 1
	newdocument.add_value(1, temp_Id);

	// Add the document to the database
        database.add_document(newdocument);

	fclose(fp_content);
	fclose(fp_id);
	}
}

char listDir(char *path, char *databaseName)
{
        FILE *file;
	char str_content[100];
	char str_id[100];
	char str_time[100];
	char statistic[100];
	char temp[100];
	char temp2[100];
	char original[100];
	int sta;

        sprintf(statistic, "%s/Statistics", path);
        if((file = fopen(statistic, "r")) != NULL){
            fscanf(file, "%*s %*s %*s %*s %*s %d", &sta);
        }
        strcpy(original, path);
        for(int i=1; i<=sta; i++){
            sprintf(temp, "%s/%d", original, i);
            for(int j=1; j<100000; j++){
                sprintf(temp2,"%s/%d", temp, j);
                puts(temp2);
                if(stat(temp2, &sb) == 0 && S_ISDIR(sb.st_mode)){
                    sprintf(str_content, "%s/content", temp2);
                    sprintf(str_id, "%s/id", temp2);
                    sprintf(str_time, "%s/time", temp2);
                    OpenFile(str_content, str_id, str_time, databaseName);
                }
                else break;
            }
        }
}

int main(int argc, char **argv)
{
	// Simplest possible options parsing: we just require three or more
    	// parameters.
    	if(argc < 3) {
        	cout << "usage: " << argv[0] <<
                	" <path to database> <document data> <document terms>" << endl;
        	return 0;
    	}

	// Catch any Xapian::Error exceptions thrown
    	try {
		// read the directory
		listDir(argv[2], argv[1]);

    	} catch(const Xapian::Error &error) {
	 	cout << "Exception: "  << error.get_msg() << endl;
    	}

	return 0;
}
