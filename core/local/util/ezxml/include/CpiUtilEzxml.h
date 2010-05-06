// -*- c++ -*-

#ifndef CPI_UTIL_EZXML_H__
#define CPI_UTIL_EZXML_H__

/**
 * \file
 * \brief Thin C++ wrapper for ezxml to ease I/O and memory management.
 *
 * Revision History:
 *
 *     09/29/2008 - Frank Pilhofer
 *                  Initial version.
 */

#include <iostream>
#include <CpiUtilVfs.h>
#include "ezxml.h"

namespace CPI {
  namespace Util {

    /**
     * \brief Defines the CPI::Util::EzXml::Doc class.
     */

    namespace EzXml {

      /**
       * \brief Convenience wrapper class for the ezxml library.
       *
       * It supports parsing XML files from various sources and takes
       * care of memory management (i.e., releasing the ezxml-allocated
       * memory in its destructor).
       */

      class Doc {
      public:
	/**
	 * Default constructor.
	 *
	 * \post This instance is unused.
	 */

	Doc ()
	  throw ();

	/**
	 * Constructor to parse XML from a string.
	 * Calls parse(\a data).
	 *
	 * After construction, call getRootNode() to get access to
	 * the document's root element.
	 *
	 * \param[in] data XML data.
	 * \throw std::string If \a data can not be parsed as an XML
	 * document.
	 *
	 * \post This instance is in use.
	 */

	Doc (const std::string & data)
	  throw (std::string);

	/**
	 * Constructor to parse XML from an input stream.
	 * Calls parse(\a in).
	 *
	 * After construction, call getRootNode() to get access to
	 * the document's root element.
	 *
	 * \param[in] in An input stream containing XML data starting at
	 * the current position.
	 * \throw std::string If an I/O error occurs, or if the data that
	 * is read from the stream can not be parsed as an XML document.
	 *
	 * \post This instance is in use.
	 */

	Doc (std::istream * in)
	  throw (std::string);

	/**
	 * Constructor to parse XML from a file.
	 * Calls parse(\a fs, \a fileName).
	 *
	 * After construction, call getRootNode() to get access to
	 * the document's root element.
	 *
	 * \param[in] fs The file system that contains the XML file.
	 * \param[in] fileName The name of the XML file within that file
	 * system.
	 * \throw std::string If an I/O error occurs, or if the data that
	 * is read from the file can not be parsed as an XML document.
	 *
	 * \post This instance is in use.
	 */

	Doc (CPI::Util::Vfs::Vfs & fs, const std::string & fileName)
	  throw (std::string);

	/**
	 * Destructor.  Releases the parsed document.
	 */

	~Doc ()
	  throw ();

	/**
	 * Parse XML from a string.
	 *
	 * \param[in] data XML data.
	 * \return The document root element.
	 * \throw std::string If \a data can not be parsed as an XML
	 * document.
	 *
	 * \pre This instance is unused.
	 * \post This instance is in use.
	 */

	ezxml_t parse (const std::string & data)
	  throw (std::string);

	/**
	 * Parse XML from an input stream.
	 *
	 * \param[in] in An input stream containing XML data starting at
	 * the current position.
	 * \return The document root element.
	 * \throw std::string If an I/O error occurs, or if the data that
	 * is read from the stream can not be parsed as an XML document.
	 *
	 * \pre This instance is unused.
	 * \post This instance is in use.
	 */

	ezxml_t parse (std::istream * in)
	  throw (std::string);

	/**
	 * Parse XML from a file.
	 *
	 * \param[in] fs The file system that contains the XML file.
	 * \param[in] fileName The name of the XML file within that file
	 * system.
	 * \return The document root element.
	 * \throw std::string If an I/O error occurs, or if the data that
	 * is read from the file can not be parsed as an XML document.
	 *
	 * \pre This instance is unused.
	 * \post This instance is in use.
	 */

	ezxml_t parse (CPI::Util::Vfs::Vfs & fs, const std::string & fileName)
	  throw (std::string);

	/**
	 * Get the document root element.
	 *
	 * \return The document root element.
	 *
	 * \pre This instance is in use.
	 */

	ezxml_t getRootNode ()
	  throw ();

      private:
	char * m_doc;
	ezxml_t m_rootNode;
      };

    }

  }
}

#endif