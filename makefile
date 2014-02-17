all: 
	g++ search.cc -o search -I/home/bruce3557/VersionedDoc/xapian/include -L/home/bruce3557/VersionedDoc/xapian/lib -lxapian
	g++ index.cc -o index -I/home/bruce3557/VersionedDoc/xapian/include -L/home/bruce3557/VersionedDoc/xapian/lib -lxapian
	
