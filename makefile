all: 
	g++ search.cc -o search -I/home/bruce3557/TraditionalBased_06/xapian/include -L/home/bruce3557/TraditionalBased_06/xapian/lib -lxapian
	g++ index.cc -o index -I/home/bruce3557/TraditionalBased_06/xapian/include -L/home/bruce3557/TraditionalBased_06/xapian/lib -lxapian
	
