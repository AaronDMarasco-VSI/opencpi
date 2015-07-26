#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <set>
#include "OcpiOsAssert.h"
#include "OcpiOsFileIterator.h"
#include "OcpiOsFileSystem.h"
#include "OcpiUtilException.h"
#include "OcpiUtilEzxml.h"
#include "OcpiLibraryManager.h"
#include "OcpiComponentLibrary.h"

// This file is the (loadable) driver for ocpi component libraries, each of which is
// rooted in a file system directory, which becomes its name

namespace OL = OCPI::Library;
namespace OA = OCPI::API;
namespace OU = OCPI::Util;
namespace OS = OCPI::OS;
namespace OCPI {
  namespace Library {
    namespace CompLib {
      using OCPI::Util::ApiError;
      class Library;

      // Our concrete artifact class
      class Artifact
	: public OL::ArtifactBase<Library, Artifact>, OU::EzXml::Doc {
	friend class Library;
      public:
	Artifact(Library &lib, const char *name, const OA::PValue *);
	~Artifact() {}
      };
	  
      class Driver;

      //      Library *g_firstLibrary;
      // Our concrete library class
      class Library : public OL::LibraryBase<Driver, Library, Artifact> {
	std::set<OS::FileSystem::FileId> m_file_ids; // unordered set cxx11 is better
	friend class Driver;
	Library(const char *name)
	  : OL::LibraryBase<Driver,Library,Artifact>(*this, name) {
	}

	void doPath(const std::string &libName) {
	  //	  ocpiDebug("Processing library path: %s", libName.c_str());
	  bool isDir;
	  OS::FileSystem::FileId file_id; 
	  if (!OS::FileSystem::exists(libName, &isDir, NULL, NULL, &file_id))
	    ocpiDebug("Component library path name in OCPI_LIBRARY_PATH, \"%s\", "
		      "is nonexistent.  It will be ignored", libName.c_str());
	  else if (m_file_ids.insert(file_id).second)
	    // New id was inserted, and thus was not already there
	    if (isDir) {
	      OS::FileIterator dir(libName, "*");
	      for (; !dir.end(); dir.next())
		doPath(OS::FileSystem::joinNames(libName, dir.relativeName()));
	    } else {
	      const char *name = libName.c_str();
	      size_t len = strlen(name), xlen = strlen(".xml");
	  
	      if (len < xlen || strcasecmp(name + len - xlen, ".xml")) {
		// FIXME: supply library level xml for the artifact
		// The log will show which files are not any good.
		try {
		  (new Artifact(*this, name, NULL))->configure();
		} catch (...) {}
	      }
	    }
	}
	public:
	  // Do a recursive directory search for all files.
	  void configure(ezxml_t) {
	    doPath(name());
	  }
	  OCPI::Library::Artifact *
	    addArtifact(const char *url, const OCPI::API::PValue *props) {
	    // return NULL if this doesn't look like an artifact we can support
	    Artifact *a = new Artifact(*this, url, props);
	    a->configure(); // FIXME: there could be config info in the platform.xml
	    return a;
	  }
	};

      // Our concrete driver class
      const char *component = "component";
      class Driver
	: public OCPI::Library::DriverBase<Driver, Library, component> {
      public:
	void configure(ezxml_t x) {
	  // First we call the base class, which loads explicit libraries.
	  OL::Driver::configure(x);
	}
	unsigned search(const PValue* /*props*/, const char **/*exclude*/,
			bool /* discoveryOnly */) {
	  unsigned n = 0;
	  // Now we look in the path environment variable
	  // FIXME: canonicalize the names before dup matching? (i.e. realpath)??
	  const char *path = getenv("OCPI_LIBRARY_PATH");
	  if (path) {
	    ocpiDebug("OCPI_LIBRARY_PATH is %s", path);
	    char *cp = strdup(path), *last;
	    try {
	      for (char *lp = strtok_r(cp, ":", &last); lp;
		   lp = strtok_r(NULL, ":", &last)) {
		// We have a library in the path.
		(new Library(lp))->configure(NULL);
		n++;
	      }
	    } catch (...) {
	      free(cp);
	      throw;
	    }
	    free(cp);
	  }
	  return n;
	}
	
	// Telling the driver to add an artifact is a forced load that does not
	// come from any other library, but the library can return NULL
	// if the file is not the type of file supported by this driver.
	OL::Artifact *addArtifact(const char *url, const OA::PValue *props) {
	  Library *l = firstChild();
	  if (!l)
	    l = new Library(".");
	  return l->addArtifact(url, props);
	}
      };
      Artifact::
      Artifact(Library &lib, const char *name, const OA::PValue *)
	: ArtifactBase<Library,Artifact>(lib, *this, name) {
	getFileMetaData(name);
      }
      RegisterLibraryDriver<Driver> driver;
    }
  }
}
