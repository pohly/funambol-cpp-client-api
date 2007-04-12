/* -*-C-*- */
/** @cond API */

/**
 * @mainpage The Funambol C++ SyncML Client Library.
 *
 * Welcome to the Funambol C++ SyncML Client Library! As the name implies,
 * this library provides an implementation of the SyncML protocol. It can
 * be compiled for Windows, Linux, Mac OS X and probably many other POSIX
 * compliant systems.
 *
 * The module list in this documentation describes everything a
 * developer needs to know to implement a SyncML client - or so we
 * hope...
 *
 * @todo A short HOWTO probably would be good.
 *
 * If some information is missing or not explained well,
 * please do not hesitate to ask for clarifications. A good starting
 * point for such inquiries is the Funambol developers mailing list:
 * https://lists.sourceforge.net/lists/listinfo/funambol-developers
 *
 * For developers working on the core library it will also be possible
 * to generate an extended version of this documentation, but this is
 * not supported yet.
 *
 * @cond DEV
 * More information for core developers:
 * @subpage Doxygen
 * @endcond
 */

/** @endcond */

/** @cond DEV */

/**
 * @page Doxygen Generating documentation with Doxygen 
 *
 * To use the Doxygen config provided in the "doc" directory for the Funambol C++ client API,
 * invoke "doxygen <full path>/Doxyfile" in the directory where doxygen is supposed to
 * generate the documentation. For this to work it must be
 * told where to find sources plus various other options, by setting the
 * following environment variables:
 * - FUNAMBOL_NATIVE_ROOT = <root directory of the C++ client library>
 * - ENABLED_SECTIONS = "API" or "API DEV" to choose between user documentation and full documentation
 * - HAVE_DOT = YES|NO to configure whether Doxygen may use the "dot" tool from the Graphviz package
 *              to generate class graphs
 *
 * Here are guidelines for writing good Doxygen comments:
 * - when implementing a derived class, do not copy the comments of the virtual methods you implement unless
 *   you want to modify them: Doxygen will automatically use the comments of the base class in all
 *   derived classes if those classes do not provide their own comments
 * - ...
 */

/** @endcond */

