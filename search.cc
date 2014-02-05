/* quickstartsearch.cc: Simplest possible searcher
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
#include <algorithm>
using namespace std;

struct Terms {
    std::string terms;
    std::string dates;
    int numb;
    int percent;
    int type;
};

vector<struct Terms> svec;
struct Terms master;

bool compareNumb(const Terms &a, const Terms &b)
{
    if(a.numb!=b.numb) return a.numb > b.numb;
    else return a.dates > b.dates;
}

int main(int argc, char **argv)
{
    // Simplest possible options parsing: we just require two or more
    // parameters.
    if (argc < 3) {
        cout << "usage: " << argv[0] <<
                " <path to database> <search terms>" << endl;
        return 0;
    }

    // Catch any Xapian::Error exceptions thrown
    try {
        // Make the database
		Xapian::Database db(argv[1]);

        // Start an enquire session
        Xapian::Enquire enquire(db);

        Xapian::MultiValueKeyMaker *keymaker = new Xapian::MultiValueKeyMaker();

        // Build the query object
        Xapian::Query query(Xapian::Query::OP_OR, argv + 2, argv + argc);
        cout << "Performing query `" << query.get_description() << "'" << endl;

        // Give the query object to the enquire session
        enquire.set_query(query);
        enquire.set_sort_by_value_then_relevance(4, true);

        // Get the top 150 results of the query
        Xapian::MSet matches = enquire.get_mset(0, 100000);

        // Display the results
        cout << matches.size() << " results found" << endl;

        printf("query: %s\n",argv[2]);
        int x=0;
        Xapian::MSetIterator j;
        for (Xapian::MSetIterator i = matches.begin(); i != matches.end(); ++i)
        {
            j = i;
            Xapian::Document doc = i.get_document();

            // Brute force approach
            Xapian::TermIterator tx = doc.termlist_begin();
            tx.skip_to(string(argv[2]));

            for(Xapian::TermIterator tx = doc.termlist_begin();tx != doc.termlist_end();++tx) {
              if( (*tx).compare(string(argv[2])) == 0) {
                //try to make a vector terms
                master.terms = doc.get_value(1);
                master.numb = tx.get_wdf();
                master.dates = doc.get_value(4);
                master.percent = i.get_percent();
                master.type = *i;
                svec.push_back(master);
                break;
              }
            }

            //sorting the freq.
            std::sort(svec.begin(), svec.end(), compareNumb);
            vector<struct Terms>::iterator iter = svec.begin();
            if( ++j == matches.end()){
                for(iter; iter != svec.end(); ++iter){
                    if(x==100) break;
                    x++;
                    cout << "Document ID " << iter->type << "    \t" << iter->dates<< "\t"<<
                                    iter->percent << "% [ " <<
                                    iter->terms << "]\t" << "tot: " << iter->numb <<endl;
                }
            }
        }

    } catch(const Xapian::Error &error) {
        cout << "Exception: "  << error.get_msg() << endl;
    }
}
