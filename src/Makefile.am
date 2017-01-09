AUTOMAKE_OPTIONS = foreign

SUBDIRS = 

bin_PROGRAMS = track
track_SOURCES = \
	Config.cc \
	Server.cc \
	Main.cc 

track_CPPFLAGS = -I$(top_srcdir) \
		$(BOOST_CPPFLAGS) \
		$(SOIL_CPPFLAGS) \
		$(CATA_CPPFLAGS) \
		$(RAPIDJSON_CPPFLAGS) \
		$(JSON_CPPFLAGS) \
		$(ZOD_CPPFLAGS)

track_LDADD = \
	$(BOOST_LIBADD) \
	$(SOIL_LIBADD) \
	$(CATA_TRADER_LIBADD) \
	$(JSON_LIBADD) \
	$(ZOD_LIBADD)


track_LDFLAGS = $(LIBADD_DL) -Wl,--no-as-needed -pthread

# header files
noinst_HEADERS = \
	Log.hh \
	Config.hh \
	Server.hh

EXTRA_DIST = env.sh track.cfg log.cfg

trackdir = $(bindir)
track_DATA = $(EXTRA_DIST)
