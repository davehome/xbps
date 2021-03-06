/*-
 * Copyright (c) 2008-2012 Juan Romero Pardines.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *-
 */

#ifndef _XBPS_API_H_
#define _XBPS_API_H_

#include <stdio.h>
#include <inttypes.h>

#include <archive.h>
#include <archive_entry.h>
#include <prop/proplib.h>
#include <confuse.h>

#ifdef  __cplusplus
# define __BEGIN_DECLS  extern "C" {
# define __END_DECLS    }
#else
# define __BEGIN_DECLS
# define __END_DECLS
#endif

/**
 * @file include/xbps_api.h
 * @brief XBPS Library API header
 *
 * This header documents the full API for the XBPS Library.
 */

/**
 * @def XBPS_PKGINDEX_VERSION
 * Current version for the repository package index format.
 */
#define XBPS_PKGINDEX_VERSION	"1.5"

#define XBPS_API_VERSION	"20120930"
#define XBPS_VERSION		"0.17"

/**
 * @def XBPS_RELVER
 * Current library release date.
 */
#define XBPS_RELVER		"XBPS: " XBPS_VERSION \
				" API: " XBPS_API_VERSION \
				" INDEX: " XBPS_PKGINDEX_VERSION

/** 
 * @def XBPS_META_PATH
 * Default root PATH to store metadata info.
 */
#define XBPS_META_PATH		"var/db/xbps"

/** 
 * @def XBPS_CACHE_PATH
 * Default cache PATH to store downloaded binpkgs.
 */
#define XBPS_CACHE_PATH		"var/cache/xbps"

/** 
 * @def XBPS_PKGDB
 * Filename for the package database.
 */
#define XBPS_PKGDB		"pkgdb.plist"

/** 
 * @def XBPS_PKGPROPS
 * Filename for package metadata property list.
 */
#define XBPS_PKGPROPS		"props.plist"

/**
 * @def XBPS_PKGFILES
 * Filename for package metadata files property list.
 */
#define XBPS_PKGFILES		"files.plist"

/** 
 * @def XBPS_PKGINDEX
 * Filename for the repository package index property list.
 */
#define XBPS_PKGINDEX		"index.plist"

/**
 * @def XBPS_PKGINDEX_FILES
 * Filename for the repository package index files property list.
 */
#define XBPS_PKGINDEX_FILES	"index-files.plist"

/**
 * @def XBPS_SYSCONF_PATH
 * Default configuration PATH to find XBPS_CONF_PLIST.
 */
#define XBPS_SYSDIR            "/xbps"
#ifndef XBPS_SYSCONF_PATH
#define XBPS_SYSCONF_PATH      "/etc" XBPS_SYSDIR
#endif

/**
 * @def XBPS_CONF_DEF
 * Filename for the XBPS plist configuration file.
 */
#define XBPS_CONF_DEF		XBPS_SYSCONF_PATH "/xbps.conf"

/**
 * @def XBPS_FLAG_VERBOSE
 * Verbose flag that can be used in the function callbacks to alter
 * its behaviour. Must be set through the xbps_handle::flags member.
 */
#define XBPS_FLAG_VERBOSE		0x00000001

/**
 * @def XBPS_FLAG_FORCE_CONFIGURE
 * Force flag used in xbps_configure_pkg(), if set the package(s)
 * will be reconfigured even if its state is XBPS_PKG_STATE_INSTALLED.
 * Must be set through the xbps_handle::flags member.
 */
#define XBPS_FLAG_FORCE_CONFIGURE	0x00000002

/**
 * @def XBPS_FLAG_FORCE_REMOVE_FILES
 * Force flag used in xbps_remove_pkg_files(), if set the package
 * files will be removed even if its SHA256 hash don't match.
 * Must be set through the xbps_handle::flags member.
 */
#define XBPS_FLAG_FORCE_REMOVE_FILES	0x00000004

/**
 * @def XBPS_FLAG_SYSLOG
 * Enable syslog logging flag. To make clients aware that syslog
 * will be used.
 */
#define XBPS_FLAG_SYSLOG		0x00000008

/**
 * @def XBPS_FLAG_INSTALL_AUTO
 * Enabled automatic install mode for a package and all dependencies
 * installed direct and indirectly.
 */
#define XBPS_FLAG_INSTALL_AUTO		0x00000010

/**
 * @def XBPS_FLAG_INSTALL_MANUAL
 * Enabled manual install mode for a package and all dependencies
 * installed direct and indirectly.
 */
#define XBPS_FLAG_INSTALL_MANUAL	0x00000020

/**
 * @def XBPS_FLAG_DEBUG
 * Enable debug mode to output debugging printfs to stdout/stderr.
 */
#define XBPS_FLAG_DEBUG 		0x00000040

/**
 * @def XBPS_FETCH_CACHECONN
 * Default (global) limit of cached connections used in libfetch.
 */
#define XBPS_FETCH_CACHECONN            6

/**
 * @def XBPS_FETCH_CACHECONN_HOST
 * Default (per host) limit of cached connections used in libfetch.
 */
#define XBPS_FETCH_CACHECONN_HOST       2

/**
 * @def XBPS_FETCH_TIMEOUT
 * Default timeout limit (in seconds) to wait for stalled connections.
 */
#define XBPS_FETCH_TIMEOUT		30

/**
 * @def XBPS_TRANS_FLUSH
 * Default number of packages to be processed in a transaction to
 * trigger a flush to the master package database XBPS_REGPKGDB.
 */
#define XBPS_TRANS_FLUSH		5

__BEGIN_DECLS

/** @addtogroup initend */ 
/*@{*/

/**
 * @enum xbps_state_t
 *
 * Integer representing the xbps callback returned state. Possible values:
 *
 * - XBPS_STATE_UKKNOWN: state hasn't been prepared or unknown error.
 * - XBPS_STATE_TRANS_DOWNLOAD: transaction is downloading binary packages.
 * - XBPS_STATE_TRANS_VERIFY: transaction is verifying binary package integrity.
 * - XBPS_STATE_TRANS_RUN: transaction is performing operations:
 * install, update, remove and replace.
 * - XBPS_STATE_TRANS_CONFIGURE: transaction is configuring all
 * unpacked packages.
 * - XBPS_STATE_DOWNLOAD: a binary package is being downloaded.
 * - XBPS_STATE_VERIFY: a binary package is being verified.
 * - XBPS_STATE_REMOVE: a package is being removed.
 * - XBPS_STATE_REMOVE_DONE: a package has been removed successfully.
 * - XBPS_STATE_REMOVE_FILE: a package file is being removed.
 * - XBPS_STATE_REMOVE_OBSOLETE: an obsolete package file is being removed.
 * - XBPS_STATE_REPLACE: a package is being replaced.
 * - XBPS_STATE_INSTALL: a package is being installed.
 * - XBPS_STATE_INSTALL_DONE: a package has been installed successfully.
 * - XBPS_STATE_UPDATE: a package is being updated.
 * - XBPS_STATE_UPDATE_DONE: a package has been updated successfully.
 * - XBPS_STATE_UNPACK: a package is being unpacked.
 * - XBPS_STATE_CONFIGURE: a package is being configured.
 * - XBPS_STATE_CONFIG_FILE: a package configuration file is being processed.
 * - XBPS_STATE_REGISTER: a package is being registered.
 * - XBPS_STATE_UNREGISTER: a package is being unregistered.
 * - XBPS_STATE_REPOSYNC: a remote repository's package index is being
 * synchronized.
 * - XBPS_STATE_VERIFY_FAIL: binary package integrity has failed.
 * - XBPS_STATE_DOWNLOAD_FAIL: binary package download has failed.
 * - XBPS_STATE_REMOVE_FAIL: a package removal has failed.
 * - XBPS_STATE_REMOVE_FILE_FAIL: a package file removal has failed.
 * - XBPS_STATE_REMOVE_FILE_HASH_FAIL: a package file removal due to
 * its hash has failed.
 * - XBPS_STATE_REMOVE_FILE_OBSOLETE_FAIL: an obsolete package file
 * removal has failed.
 * - XBPS_STATE_CONFIGURE_FAIL: package configure has failed.
 * - XBPS_STATE_CONFIG_FILE_FAIL: package configuration file operation
 * has failed.
 * - XBPS_STATE_UPDATE_FAIL: package update has failed.
 * - XBPS_STATE_UNPACK_FAIL: package unpack has failed.
 * - XBPS_STATE_REGISTER_FAIL: package register has failed.
 * - XBPS_STATE_UNREGISTER_FAIL: package unregister has failed.
 * - XBPS_STATE_REPOSYNC_FAIL: syncing remote repositories has failed.
 */
typedef enum xbps_state {
	XBPS_STATE_UNKNOWN = 0,
	XBPS_STATE_TRANS_DOWNLOAD,
	XBPS_STATE_TRANS_VERIFY,
	XBPS_STATE_TRANS_RUN,
	XBPS_STATE_TRANS_CONFIGURE,
	XBPS_STATE_DOWNLOAD,
	XBPS_STATE_VERIFY,
	XBPS_STATE_REMOVE,
	XBPS_STATE_REMOVE_DONE,
	XBPS_STATE_REMOVE_FILE,
	XBPS_STATE_REMOVE_FILE_OBSOLETE,
	XBPS_STATE_PURGE,
	XBPS_STATE_PURGE_DONE,
	XBPS_STATE_REPLACE,
	XBPS_STATE_INSTALL,
	XBPS_STATE_INSTALL_DONE,
	XBPS_STATE_UPDATE,
	XBPS_STATE_UPDATE_DONE,
	XBPS_STATE_UNPACK,
	XBPS_STATE_CONFIGURE,
	XBPS_STATE_CONFIG_FILE,
	XBPS_STATE_REGISTER,
	XBPS_STATE_UNREGISTER,
	XBPS_STATE_REPOSYNC,
	XBPS_STATE_VERIFY_FAIL,
	XBPS_STATE_DOWNLOAD_FAIL,
	XBPS_STATE_REMOVE_FAIL,
	XBPS_STATE_REMOVE_FILE_FAIL,
	XBPS_STATE_REMOVE_FILE_HASH_FAIL,
	XBPS_STATE_REMOVE_FILE_OBSOLETE_FAIL,
	XBPS_STATE_PURGE_FAIL,
	XBPS_STATE_CONFIGURE_FAIL,
	XBPS_STATE_CONFIG_FILE_FAIL,
	XBPS_STATE_UPDATE_FAIL,
	XBPS_STATE_UNPACK_FAIL,
	XBPS_STATE_REGISTER_FAIL,
	XBPS_STATE_UNREGISTER_FAIL,
	XBPS_STATE_REPOSYNC_FAIL
} xbps_state_t;

/**
 * @struct xbps_state_cb_data xbps_api.h "xbps_api.h"
 * @brief Structure to be passed as argument to the state function callback.
 */
struct xbps_state_cb_data {
	/**
	 * @var state
	 *
	 * Returned xbps state (set internally, read-only).
	 */
	xbps_state_t state;
	/**
	 * @var desc
	 *
	 * XBPS state string description (set internally, read-only).
	 */
	const char *desc;
	/**
	 * @var pkgname
	 *
	 * Package name string (set internally, read-only).
	 */
	const char *pkgname;
	/**
	 * @var version
	 *
	 * Package version string (set internally, read-only).
	 */
	const char *version;
	/**
	 * @var err
	 *
	 * XBPS state error value (set internally, read-only).
	 */
	int err;
};

/**
 * @struct xbps_fetch_cb_data xbps_api.h "xbps_api.h"
 * @brief Structure to be passed to the fetch function callback.
 *
 * This structure is passed as argument to the fetch progress function
 * callback and its members will be updated when there's any progress.
 * All members marked as read-only in this struct are set internally by
 * xbps_unpack_binary_pkg() and shouldn't be modified in the passed
 * function callback.
 */
struct xbps_fetch_cb_data {
	/**
	 * @var file_size
	 *
	 * Filename size for the file to be fetched.
	 */
	off_t file_size;
	/**
	 * @var file_offset
	 *
	 * Current offset for the filename being fetched.
	 */
	off_t file_offset;
	/**
	 * @var file_dloaded
	 *
	 * Bytes downloaded for the file being fetched.
	 */
	off_t file_dloaded;
	/**
	 * @var file_name
	 *
	 * File name being fetched.
	 */
	const char *file_name;
	/**
	 * @var cb_start
	 *
	 * If true the function callback should be prepared to start
	 * the transfer progress.
	 */
	bool cb_start;
	/**
	 * @var cb_update
	 *
	 * If true the function callback should be prepared to
	 * update the transfer progress.
	 */
	bool cb_update;
	/**
	 * @var cb_end
	 *
	 * If true the function callback should be prepated to
	 * end the transfer progress.
	 */
	bool cb_end;
};

/**
 * @struct xbps_unpack_cb_data xbps_api.h "xbps_api.h"
 * @brief Structure to be passed to the unpack function callback.
 *
 * This structure is passed as argument to the unpack progress function
 * callback and its members will be updated when there's any progress.
 * All members in this struct are set internally by xbps_unpack_binary_pkg()
 * and should be used in read-only mode in the function callback.
 * The \a cookie member can be used to pass user supplied data.
 */
struct xbps_unpack_cb_data {
	/**
	 * @var pkgver
	 *
	 * Package name/version string of package being unpacked.
	 */
	const char *pkgver;
	/**
	 * @var entry
	 *
	 * Entry pathname string.
	 */
	const char *entry;
	/**
	 * @var entry_size
	 *
	 * Entry file size.
	 */
	int64_t entry_size;
	/**
	 * @var entry_extract_count
	 *
	 * Total number of extracted entries.
	 */
	ssize_t entry_extract_count;
	/**
	 * @var entry_total_count
	 *
	 * Total number of entries in package.
	 */
	ssize_t entry_total_count;
	/**
	 * @var entry_is_conf
	 *
	 * If true "entry" is a configuration file.
	 */
	bool entry_is_conf;
};

/**
 * @struct xbps_handle xbps_api.h "xbps_api.h"
 * @brief Generic XBPS structure handler for initialization.
 *
 * This structure sets some global properties for libxbps, to set some
 * function callbacks and data to the fetch, transaction and unpack functions,
 * the root and cache directory, flags, etc.
 */
struct xbps_handle {
	/**
	 * @private
	 */
	cfg_t *cfg;
	/**
	 * @private
	 *
	 * Array of dictionaries with all registered repositories.
	 */
	prop_array_t repo_pool;
	/**
	 * @private pkgdb.
	 *
	 * Internalized proplib array with the master package database
	 * stored in XBPS_META_PATH/XBPS_PKGDB.
	 */
	prop_array_t pkgdb;
	/**
	 * @var transd;
	 *
	 * Proplib dictionary with transaction details, required by
	 * xbps_transaction_commit().
	 */
	prop_dictionary_t transd;
	/**
	 * Pointer to the supplifed function callback to be used
	 * in the XBPS possible states.
	 */
	void (*state_cb)(struct xbps_handle *, struct xbps_state_cb_data *, void *);
	/**
	 * Pointer to user supplied data to be passed as argument to
	 * the \a xbps_state_cb function callback.
	 */
	void *state_cb_data;
	/**
	 * Pointer to the supplied function callback to be used in
	 * xbps_unpack_binary_pkg().
	 */
	void (*unpack_cb)(struct xbps_handle *, struct xbps_unpack_cb_data *, void *);
	/**
	 * Pointer to user supplied data to be passed as argument to
	 * the \a xbps_unpack_cb function callback.
	 */
	void *unpack_cb_data;
	/**
	 * Pointer to the supplied function callback to be used in
	 * xbps_fetch_file().
	 */
	void (*fetch_cb)(struct xbps_handle *, struct xbps_fetch_cb_data *, void *);
	/**
	 * @var fetch_cb_data
	 *
	 * Pointer to user supplied data to be passed as argument to
	 * the \a xbps_fetch_cb function callback.
	 */
	void *fetch_cb_data;
	/**
	 * @var rootdir
	 *
	 * Root directory for all operations in XBPS. If NULL,
	 * by default it's set to /.
	 */
	const char *rootdir;
	/**
	 * @var cachedir
	 *
	 * Cache directory to store downloaded binary packages.
	 * If NULL default value in \a XBPS_CACHE_PATH is used.
	 */
	const char *cachedir;
	/**
	 * @var metadir
	 *
	 * Metadata directory for all operations in XBPS.
	 * If NULL, defaults to \a XBPS_CACHE_PATH (relative to rootdir).
	 */
	const char *metadir;
	/**
	 * @private
	 */
	char *cachedir_priv;
	char *metadir_priv;
	char *un_machine;
	/*
	 * @var repository
	 *
	 * Default repository to be used if a configuration file
	 * couldn't be found.
	 */
	const char *repository;
	/**
	 * @var conffile
	 *
	 * Full path to the xbps configuration file.
	 */
	const char *conffile;
	/**
	 * @var fetch_timeout
	 *
	 * Unsigned integer to specify libfetch's timeout limit.
	 * If not set, it defaults to 30 (in seconds). This is set internally
	 * by the API from a setting in configuration file.
	 */
	uint16_t fetch_timeout;
	/**
	 * @var transaction_frequency_flush
	 *
	 * Number of packages to be processed in a transaction to
	 * trigger a flush to the master databases.
	 */
	uint16_t transaction_frequency_flush;
	/**
	 * @var flags
	 *
	 * Flags to be set globally by ORing them, possible values:
	 *  - XBPS_FLAG_VERBOSE
	 *  - XBPS_FLAG_FORCE_CONFIGURE
	 *  - XBPS_FLAG_FORCE_REMOVE_FILES
	 *  - XBPS_FLAG_DEBUG
	 *  - XBPS_FLAG_SYSLOG
	 *  - XBPS_FLAG_INSTALL_AUTO
	 *  - XBPS_FLAG_INSTALL_MANUAL
	 */
	int flags;
	/**
	 * @private
	 */
	bool initialized;
};

void xbps_dbg_printf(struct xbps_handle *, const char *, ...);
void xbps_dbg_printf_append(struct xbps_handle *, const char *, ...);
void xbps_error_printf(const char *, ...);
void xbps_warn_printf(const char *, ...);

/**
 * Initialize the XBPS library with the following steps:
 *
 *   - Set function callbacks for fetching and unpacking.
 *   - Set default cache connections for libfetch.
 *   - Parse configuration file.
 *
 * @param[in] xhp Pointer to an xbps_handle struct.
 * @note It's assumed that \a xhp is a valid pointer.
 *
 * @return 0 on success, an errno value otherwise.
 */
int xbps_init(struct xbps_handle *xhp);

/**
 * Releases all resources used by libxbps.
 *
 * @param[in] xhp Pointer to an xbps_handle struct.
 */
void xbps_end(struct xbps_handle *xhp);

/*@}*/

/** @addtogroup configure */
/*@{*/

/**
 * Configure (or force reconfiguration of) a package.
 *
 * @param[in] xhp Pointer to an xbps_handle struct.
 * @param[in] pkgname Package name to configure.
 * @param[in] check_state Set it to true to check that package is
 * in unpacked state.
 * @param[in] update Set it to true if this package is being updated.
 * @param[in] flush Set it to true to flush state to pkgdb.
 *
 * @return 0 on success, otherwise an errno value.
 */
int xbps_configure_pkg(struct xbps_handle *xhp,
		       const char *pkgname,
		       bool check_state,
		       bool update,
		       bool flush);

/**
 * Configure (or force reconfiguration of) all packages.
 *
 * @param[in] xhp Pointer to an xbps_handle struct.
 * @param[in] flush Set it to true to flush state to pkgdb.
 *
 * @return 0 on success, otherwise an errno value.
 */
int xbps_configure_packages(struct xbps_handle *xhp, bool flush);

/*@}*/

/** @addtogroup download */
/*@{*/


/**
 * Download a file from a remote URL.
 * 
 * @param[in] xhp Pointer to an xbps_handle struct.
 * @param[in] uri Remote URI string.
 * @param[in] outputdir Directory string to store downloaded file.
 * @param[in] refetch If true and local/remote size/mtime do not match,
 * fetch the file from scratch.
 * @param[in] flags Flags passed to libfetch's fetchXget().
 * 
 * @return -1 on error, 0 if not downloaded (because local/remote size/mtime
 * do not match) and 1 if downloaded successfully.
 **/
int xbps_fetch_file(struct xbps_handle *xhp,
		    const char *uri,
		    const char *outputdir,
		    bool refetch,
		    const char *flags);

/**
 * Returns last error string reported by xbps_fetch_file().
 *
 * @return A string with the appropiate error message.
 */
const char *xbps_fetch_error_string(void);

/*@}*/

/**
 * @ingroup pkg_orphans
 *
 * Finds all package orphans currently installed.
 *
 * @param[in] xhp Pointer to an xbps_handle struct.
 * @param[in] orphans Proplib array of strings with package names of
 * packages that should be treated as they were already removed (optional).
 *
 * @return A proplib array of dictionaries with all orphans found,
 * on error NULL is returned and errno is set appropiately.
 */
prop_array_t xbps_find_pkg_orphans(struct xbps_handle *xhp, prop_array_t orphans);

/** @addtogroup pkgdb */
/*@{*/

/**
 * Executes a function callback per a package dictionary registered
 * in master package database (pkgdb) plist (downwards).
 *
 * @param[in] xhp The pointer to the xbps_handle struct.
 * @param[in] fn Function callback to run for any pkg dictionary.
 * @param[in] arg Argument to be passed to the function callback.
 *
 * @return 0 on success (all objects were processed), otherwise
 * the value returned by the function callback.
 */
int xbps_pkgdb_foreach_cb(struct xbps_handle *xhp,
	int (*fn)(struct xbps_handle *, prop_object_t, void *, bool *),
	void *arg);

/**
 * Executes a function callback per a package dictionary registered
 * in master package database (pkgdb) plist, in reverse order (upwards).
 *
 * @param[in] xhp The pointer to the xbps_handle struct.
 * @param[in] fn Function callback to run for any pkg dictionary.
 * @param[in] arg Argument to be passed to the function callback.
 *
 * @return 0 on success (all objects were processed), otherwise
 * the value returned by the funcion callback.
 */
int xbps_pkgdb_foreach_reverse_cb(
		struct xbps_handle *xhp,
		int (*fn)(struct xbps_handle *, prop_object_t, void *, bool *),
		void *arg);

/**
 * Returns a package dictionary from master package database (pkgdb) plist,
 * matching pkgname or pkgver object in \a pkg.
 *
 * @param[in] xhp The pointer to the xbps_handle struct.
 * @param[in] pkg Package name or name-version to match.
 * @param[in] bypattern If false \a pkg must be a pkgname, otherwise a
 * package pattern, i.e `foo>=0' or `foo<1'.
 *
 * @return The matching proplib package dictionary, NULL otherwise.
 */
prop_dictionary_t xbps_pkgdb_get_pkgd(struct xbps_handle *xhp,
				      const char *pkg,
				      bool bypattern);

/**
 * Returns a package dictionary from master package database (pkgdb) plist,
 * matching the pkgver object in \a pkg dictionary.
 *
 * @param[in] xhp The pointer to the xbps_handle struct.
 * @param[in] pkgver Package name-version to match, i.e 'foo-1.0'.
 *
 * @return The matching proplib package dictionary, NULL otherwise.
 */
prop_dictionary_t xbps_pkgdb_get_pkgd_by_pkgver(struct xbps_handle *xhp,
						const char *pkgver);

/**
 * Removes a package dictionary from master package database (pkgdb) plist,
 * matching pkgname or pkgver object in \a pkg.
 *
 * @param[in] xhp The pointer to the xbps_handle struct.
 * @param[in] pkg Package name or pattern to match in a package dictionary.
 * @param[in] bypattern If false \a pkg must be a pkgname, otherwise a
 * package pattern, i.e `foo>=0' or `foo<1'.
 * @param[in] flush If true, after successful replace the pkgdb contents
 * in memory will be flushed atomically to storage.
 *
 * @return true on success, false otherwise.
 */
bool xbps_pkgdb_remove_pkgd(struct xbps_handle *xhp,
			    const char *pkg,
			    bool bypattern,
			    bool flush);

/**
 * Replaces a package dictionary with \a dict in the master package database
 * (pkgdb) plist, matching pkgname or pkgver object.
 *
 * @param[in] xhp The pointer to the xbps_handle struct.
 * @param[in] pkgd Proplib dictionary to be added into pkgdb.
 * @param[in] pkg Package name or pattern to match in a package dictionary.
 * @param[in] bypattern If false \a pkg must be a pkgname, otherwise a
 * package pattern, i.e `foo>=0' or `foo<1'.
 * @param[in] flush If true, after successful replace the pkgdb contents in
 * memory will be flushed atomically to storage.
 *
 * @return true on success, false otherwise.
 */
bool xbps_pkgdb_replace_pkgd(struct xbps_handle *xhp,
			     prop_dictionary_t pkgd,
			     const char *pkg,
			     bool bypattern,
			     bool flush);

/**
 * Updates the master package database (pkgdb) plist with new contents from
 * disk to the cached copy in memory.
 *
 * @param[in] xhp The pointer to the xbps_handle struct.
 * @param[in] flush If true the pkgdb plist contents in memory will
 * be flushed atomically to storage.
 *
 * @return 0 on success, otherwise an errno value.
 */
int xbps_pkgdb_update(struct xbps_handle *xhp, bool flush);

/*@}*/

/** @addtogroup plist */
/*@{*/

/**
 * Adds a proplib object into a proplib dictionary with specified key.
 * The object is always released.
 *
 * @param[in] dict Proplib dictionary to insert the object to.
 * @param[in] obj Proplib object to be inserted.
 * @param[in] key Key associated with \a obj.
 *
 * @return true on success, false otherwise and errno set appropiately.
 */
bool xbps_add_obj_to_dict(prop_dictionary_t dict,
			  prop_object_t obj,
			  const char *key);

/**
 * Adds a proplib object into a proplib array.
 * The object is always released.
 *
 * @param[in] array Proplib array to insert the object to.
 * @param[in] obj Proplib object to be inserted.
 * 
 * @return true on success, false otherwise and xbps_errno set appropiately.
 */
bool xbps_add_obj_to_array(prop_array_t array, prop_object_t obj);

/**
 * Executes a function callback specified in \a fn with \a arg passed
 * as its argument into they array \a array.
 *
 * @param[in] xhp The pointer to the xbps_handle struct.
 * @param[in] array Proplib array to iterate.
 * @param[in] fn Function callback to run on every object in the array.
 * While running the function callback, the hird parameter (a pointer to
 * a boolean) can be set to true to stop immediately the loop.
 * @param[in] arg Argument to be passed to the function callback.
 *
 * @return 0 on success, otherwise the value returned by the function
 * callback.
 */
int xbps_callback_array_iter(struct xbps_handle *xhp,
	prop_array_t array,
	int (*fn)(struct xbps_handle *, prop_object_t, void *, bool *),
	void *arg);

/**
 * Executes a function callback specified in \a fn with \a arg passed
 * as its argument into they array \a array in reverse order (upwards).
 *
 * @param[in] xhp The pointer to the xbps_handle struct.
 * @param[in] array Proplib array to iterate.
 * @param[in] fn Function callback to run on every object in the array.
 * While running the function callback, the hird parameter (a pointer to
 * a boolean) can be set to true to stop immediately the loop.
 * @param[in] arg Argument to be passed to the function callback.
 *
 * @return 0 on success, otherwise the value returned by the function
 * callback.
 */
int xbps_callback_array_iter_reverse(struct xbps_handle *xhp,
	prop_array_t array,
	int (*fn)(struct xbps_handle *, prop_object_t, void *, bool *),
	void *arg);

/**
 * Executes a function callback into the array associated with key \a key,
 * contained in a proplib dictionary.
 *
 * @param[in] xhp The pointer to the xbps_handle struct.
 * @param[in] dict Proplib dictionary where the array resides.
 * @param[in] key Key associated with array.
 * @param[in] fn Function callback to run on every
 * object in the array. While running the function callback, the third
 * parameter (a pointer to a boolean) can be set to true to stop
 * immediately the loop.
 * @param[in] arg Argument to be passed to the function callback.
 *
 * @return 0 on success (all objects were processed), otherwise
 * the value returned by the function callback.
 */
int xbps_callback_array_iter_in_dict(struct xbps_handle *xhp,
	prop_dictionary_t dict,
	const char *key,
	int (*fn)(struct xbps_handle *, prop_object_t, void *, bool *),
	void *arg);

/**
 * Executes a function callback (in reverse order) into the array
 * associated with key \a key, contained in a proplib dictionary.
 *
 * @param[in] xhp The pointer to the xbps_handle struct.
 * @param[in] dict Proplib dictionary where the array resides.
 * @param[in] key Key associated with array.
 * @param[in] fn Function callback to run on every
 * object in the array. While running the function callback, the third
 * parameter (a pointer to a boolean) can be set to true to stop
 * immediately the loop.
 * @param[in] arg Argument to be passed to the function callback.
 *
 * @return 0 on success (all objects were processed), otherwise
 * the value returned by the function callback.
 */
int xbps_callback_array_iter_reverse_in_dict(struct xbps_handle *xhp,
	prop_dictionary_t dict,
	const char *key,
	int (*fn)(struct xbps_handle *, prop_object_t, void *, bool *),
	void *arg);

/**
 * Finds the proplib's dictionary associated with a package, by looking
 * it via its name in a proplib dictionary.
 *
 * @param[in] xhp The pointer to the xbps_handle struct.
 * @param[in] dict Proplib dictionary to look for the package dictionary.
 * @param[in] key Key associated with the array that stores package's dictionary.
 * @param[in] pkgname Package name to look for.
 *
 * @return The package's proplib dictionary on success, NULL otherwise and
 * errno is set appropiately.
 */
prop_dictionary_t xbps_find_pkg_in_dict_by_name(struct xbps_handle *xhp,
						prop_dictionary_t dict,
						const char *key,
						const char *pkgname);

/**
 * Finds the proplib's dictionary associated with a package, by looking
 * at it via a package pattern in a proplib dictionary.
 *
 * @param[in] xhp The pointer to the xbps_handle struct.
 * @param[in] dict Proplib dictionary to look for the package dictionary.
 * @param[in] key Key associated with the array storing the package's dictionary.
 * @param[in] pattern Package pattern to match.
 *
 * @return The package's proplib dictionary on success, NULL otherwise
 * and errno is set appropiately.
 */
prop_dictionary_t xbps_find_pkg_in_dict_by_pattern(struct xbps_handle *xhp,
						   prop_dictionary_t dict,
						   const char *key,
						   const char *pattern);

/**
 * Finds the proplib's dictionary associated with a package, by matching
 * its \a pkgver object in its dictionary.
 *
 * @param[in] xhp The pointer to the xbps_handle struct.
 * @param[in] dict Proplib dictionary to look for the package dictionary.
 * @param[in] key Key associated with the array storing the package's
 * dictionary.
 * @param[in] pkgver Package name/version to match, i.e `foo-1.0_1'.
 *
 * @return The package's proplib dictionary on success, NULL otherwise
 * and errno is set appropiately.
 */
prop_dictionary_t xbps_find_pkg_in_dict_by_pkgver(struct xbps_handle *xhp,
						  prop_dictionary_t dict,
						  const char *key,
						  const char *pkgver);

/**
 * Finds the proplib's dictionary associated with a package, by matching
 * a pkgname in \a name on any of the virtual package in
 * the "provides" array object.
 *
 * @param[in] xhp The pointer to the xbps_handle struct.
 * @param[in] d Proplib dictionary to look for the package dictionary.
 * @param[in] key Key associated with the array storing the package's
 * dictionary.
 * @param[in] name The virtual package name to match.
 *
 * @return The package dictionary, otherwise NULL is returned and errno
 * is set appropiately.
 * Finds a virtual package dictionary in a proplib array by matching a
 * package name.
 */
prop_dictionary_t xbps_find_virtualpkg_in_dict_by_name(struct xbps_handle *xhp,
						       prop_dictionary_t d,
						       const char *key,
						       const char *name);

/**
 * Finds the proplib's dictionary associated with a package, by matching
 * a pkg pattern in \a pattern on any of the virtual package in
 * the "provides" array object.
 *
 * @param[in] xhp The pointer to the xbps_handle struct.
 * @param[in] d Proplib dictionary to look for the package dictionary.
 * @param[in] key Key associated with the array storing the package's
 * dictionary.
 * @param[in] pattern The virtual package pattern to match, i.e
 * `foo>=0' or `foo<1'.
 *
 * @return The package dictionary, otherwise NULL is returned and errno
 * is set appropiately.
 */
prop_dictionary_t xbps_find_virtualpkg_in_dict_by_pattern(struct xbps_handle *xhp,
							  prop_dictionary_t d,
							  const char *key,
							  const char *pattern);

/**
 * Finds a package's dictionary searching in the registered packages
 * database by using a package name or a package pattern.
 *
 * @param[in] xhp The pointer to the xbps_handle struct.
 * @param[in] str Package name or package pattern.
 * @param[in] bypattern Set it to true to find the package dictionary
 * by using a package pattern. If false, \a str is assumed to be a package name.
 *
 * @return The package's dictionary on success, NULL otherwise and
 * errno is set appropiately.
 */
prop_dictionary_t xbps_find_pkg_dict_installed(struct xbps_handle *xhp,
					       const char *str,
					       bool bypattern);


/**
 * Finds a virtual package's dictionary searching in the registered packages
 * database by using a package name or a package pattern.
 *
 * @param[in] xhp The pointer to the xbps_handle struct.
 * @param[in] str Virtual package name or package pattern to match.
 * @param[in] bypattern Set it to true to find the package dictionary
 * by using a package pattern. If false, \a str is assumed to be a package name.
 *
 * @return The virtual package's dictionary on success, NULL otherwise and
 * errno is set appropiately.
 */
prop_dictionary_t xbps_find_virtualpkg_dict_installed(struct xbps_handle *xhp,
						      const char *str,
						      bool bypattern);

/**
 * Match a virtual package name or pattern by looking at package's
 * dictionary "provides" array object.
 *
 * @param[in] pkgd Package dictionary.
 * @param[in] str Virtual package name or package pattern to match.
 * @param[in] bypattern If true, \a str should be a package name,
 * otherwise it should be a package pattern, i.e `foo>=0' or `foo<1'.
 *
 * @return True if \a str matches a virtual package in \a pkgd, false
 * otherwise.
 */
bool xbps_match_virtual_pkg_in_dict(prop_dictionary_t pkgd,
				   const char *str,
				   bool bypattern);

/**
 * Match any virtual package from array \a provides in they array \a rundeps
 * with dependencies.
 *
 * @param[in] rundeps Proplib array with dependencies as strings, i.e foo>=2.0.
 * @param[in] provides Proplib array of strings with virtual pkgdeps, i.e
 * foo-1.0 blah-2.0.
 *
 * @return True if \a any virtualpkg has been matched, false otherwise.
 */
bool xbps_match_any_virtualpkg_in_rundeps(prop_array_t rundeps,
					  prop_array_t provides);

/**
 * Finds a package dictionary in a proplib array by matching a package name.
 *
 * @param[in] xhp The pointer to the xbps_handle struct.
 * @param[in] array The proplib array to search on.
 * @param[in] name The package name to match.
 * @param[in] targetarch If set, package will be matched against this
 * architecture.
 *
 * @return The package dictionary, otherwise NULL is returned.
 */
prop_dictionary_t xbps_find_pkg_in_array_by_name(struct xbps_handle *xhp,
						 prop_array_t array,
						 const char *name,
						 const char *targetarch);

/**
 * Finds a package dictionary in a proplib array by matching a package pattern.
 *
 * @param[in] xhp The pointer to the xbps_handle struct.
 * @param[in] array The proplib array to search on.
 * @param[in] pattern The package pattern to match, i.e `foo>=0' or `foo<1'.
 * @param[in] targetarch If set, package will be matched against this
 * architecture.
 *
 * @return The package dictionary, otherwise NULL is returned.
 */
prop_dictionary_t xbps_find_pkg_in_array_by_pattern(struct xbps_handle *xhp,
						    prop_array_t array,
						    const char *pattern,
						    const char *targetarch);

/**
 * Finds a package dictionary in a proplib array by matching a \a pkgver
 * object.
 *
 * @param[in] xhp The pointer to the xbps_handle struct.
 * @param[in] array The proplib array to search on.
 * @param[in] pkgver The package name/version to match, i.e `foo-1.0'.
 * @param[in] targetarch If set, package will be matched against this
 * architecture.
 *
 * @return The package dictionary, otherwise NULL is returned.
 */
prop_dictionary_t xbps_find_pkg_in_array_by_pkgver(struct xbps_handle *xhp,
						   prop_array_t array,
						   const char *pkgver,
						   const char *targetarch);

/**
 * Finds a virtual package dictionary in a proplib array by matching a
 * package name.
 *
 * @param[in] xhp The pointer to the xbps_handle struct.
 * @param[in] array The proplib array to search on.
 * @param[in] name The virtual package name to match.
 *
 * @return The package dictionary, otherwise NULL is returned.
 */
prop_dictionary_t xbps_find_virtualpkg_in_array_by_name(struct xbps_handle *xhp,
							prop_array_t array,
							const char *name);

/**
 * Finds a virtual package dictionary in a proplib array by matching a
 * package pattern.
 *
 * @param[in] xhp The pointer to the xbps_handle struct.
 * @param[in] array The proplib array to search on.
 * @param[in] pattern The virtual package pattern to match, i.e
 * `foo>=0' or `foo<1'.
 *
 * @return The package dictionary, otherwise NULL is returned.
 */
prop_dictionary_t
	xbps_find_virtualpkg_in_array_by_pattern(struct xbps_handle *xhp,
						 prop_array_t array,
						 const char *pattern);

/**
 * Match a package name in the specified array of strings.
 *
 * @param[in] array The proplib array to search on.
 * @param[in] pkgname The package name to match.
 *
 * @return true on success, false otherwise and errno is set appropiately.
 */
bool xbps_match_pkgname_in_array(prop_array_t array, const char *pkgname);

/**
 * Match a package pattern in the specified array of strings.
 *
 * @param[in] array The proplib array to search on.
 * @param[in] pattern The package pattern to match, i.e `foo>=0' or `foo<1'.
 *
 * @return true on success, false otherwise and errno is set appropiately.
 */
bool xbps_match_pkgpattern_in_array(prop_array_t array, const char *pattern);

/**
 * Match a package dependency against any package pattern in the specified
 * array of strings.
 *
 * @param[in] array The proplib array to search on.
 * @param[in] pkgver The package name-version to match, i.e `foo-1.0'.
 *
 * @return true on success, false otherwise and errno is set appropiately.
 */
bool xbps_match_pkgdep_in_array(prop_array_t array, const char *pkgver);

/**
 * Match a string (exact match) in the specified array of strings.
 *
 * @param[in] array The proplib array to search on.
 * @param[in] val The string to be matched.
 *
 * @return true on success, false otherwise and errno is set appropiately.
 */
bool xbps_match_string_in_array(prop_array_t array, const char *val);

/**
 * Returns a proplib object iterator associated with an array, contained
 * in a proplib dictionary matching the specified key.
 *
 * @param[in] dict Proplib dictionary where to look for the array.
 * @param[in] key Key associated with the array.
 *
 * @return A proplib object iterator on success, NULL otherwise and
 * errno is set appropiately.
 */
prop_object_iterator_t xbps_array_iter_from_dict(prop_dictionary_t dict,
						 const char *key);

/**
 * Returns a proplib object dictionary associated with the installed package
 * \a pkgname, by internalizing its plist file defined in \a plist.
 *
 * @param[in] xhp The pointer to the xbps_handle struct.
 * @param[in] pkgname Package name of installed package.
 * @param[in] plist Package metadata property list file.
 *
 * @return The proplib object dictionary on success, NULL otherwise and
 * errno is set appropiately.
 */
prop_dictionary_t xbps_dictionary_from_metadata_plist(struct xbps_handle *xhp,
						      const char *pkgname,
						      const char *plist);

/**
 * Removes the package's proplib dictionary matching \a pkgname
 * in a proplib array.
 *
 * @param[in] xhp The pointer to the xbps_handle struct.
 * @param[in] array Proplib array where to look for.
 * @param[in] name Package name to match in the array.
 * @param[in] targetarch If set, package will be matched against this
 * architecture.
 *
 * @return true on success, false otherwise and errno is set appropiately.
 */
bool xbps_remove_pkg_from_array_by_name(struct xbps_handle *xhp,
					prop_array_t array,
					const char *name,
					const char *targetarch);

/**
 * Removes the package's proplib dictionary matching the pkgver object
 * with a package pattern from \a pattern in a proplib array.
 *
 * @param[in] xhp The pointer to the xbps_handle struct.
 * @param[in] array Proplib array where to look for.
 * @param[in] pattern Package pattern to match, i.e `foo>=0' or `foo<1'.
 * @param[in] targetarch If set, package will be matched against this
 * architecture.
 *
 * @return true on success, false otherwise and errno is set appropiately.
 */
bool xbps_remove_pkg_from_array_by_pattern(struct xbps_handle *xhp,
					   prop_array_t array,
					   const char *pattern,
					   const char *targetarch);

/**
 * Removes the package's proplib dictionary matching the \a pkgver
 * object in a proplib array of dictionaries.
 *
 * @param[in] xhp The pointer to the xbps_handle struct.
 * @param[in] array Proplib array where to look for.
 * @param[in] pkgver Package name/version to match, i.e `foo-1.0'.
 * @param[in] targetarch If set, package will be matched against this
 * architecture.
 *
 * @return true on success, false otherwise and errno is set appropiately.
 */
bool xbps_remove_pkg_from_array_by_pkgver(struct xbps_handle *xhp,
					  prop_array_t array,
					  const char *pkgver,
					  const char *targetarch);

/**
 * Removes a string from a proplib's array of strings.
 *
 * @param[in] xhp The pointer to the xbps_handle struct.
 * @param[in] array Proplib array where to look for.
 * @param[in] str String to match in the array.
 *
 * @return true on success, false otherwise and errno is set appropiately.
 */
bool xbps_remove_string_from_array(struct xbps_handle *xhp,
				   prop_array_t array,
				   const char *str);

/**
 * Removes a string from a proplib's array matched by a package name.
 *
 * @param[in] xhp The pointer to the xbps_handle struct.
 * @param[in] array Proplib array where to look for.
 * @param[in] name Package name to match.
 *
 * @return true on success, false otherwise and errno is set appropiately.
 */
bool xbps_remove_pkgname_from_array(struct xbps_handle *xhp,
				    prop_array_t array,
				    const char *name);

/**
 * Replaces a dictionary with another dictionary in \a dict, in the
 * array \a array by matching its "pkgname" object with \a pkgname.
 *
 * @param[in] array Proplib array where to look for.
 * @param[in] dict Proplib dictionary to be added in \a array.
 * @param[in] pkgname Package name to be matched.
 *
 * @retval 0 success.
 * @retval EINVAL Dictionary couldn't be set in array.
 * @retval ENOENT No match.
 */
int xbps_array_replace_dict_by_name(prop_array_t array,
				    prop_dictionary_t dict,
				    const char *pkgname);

/**
 * Replaces a dictionary with another dictionary in \a dict, in
 * the array \a array by matching its pkgver object with a package pattern,
 * i.e `foo>=0' or `foo<1'.
 *
 * @param[in] array Proplib array where to look for.
 * @param[in] dict Proplib dictionary to be added in \a array.
 * @param[in] pattern Package pattern to be matched, i.e `foo>=0'.
 *
 * @retval 0 success.
 * @retval EINVAL Dictionary couldn't be set in array.
 * @retval ENOENT No match.
 */
int xbps_array_replace_dict_by_pattern(prop_array_t array,
				       prop_dictionary_t dict,
				       const char *pattern);

/*@}*/

/** @addtogroup pkg_register */
/*@{*/

/**
 * Register a package into the installed packages database.
 *
 * @param[in] xhp The pointer to the xbps_handle struct.
 * @param[in] pkg_dict A dictionary with the following objects:
 * \a pkgname, \a version, \a pkgver, \a short_desc (string),
 * \a automatic-install (bool) and optionally \a provides (array of strings).
 * @param[in] flush Set to true to make sure that pkgdb plist
 * is written to storage on success.
 *
 * @return 0 on success, otherwise an errno value.
 */
int xbps_register_pkg(struct xbps_handle *xhp,
		      prop_dictionary_t pkg_dict,
		      bool flush);

/**
 * Unregister a package from the package database.
 *
 * @param[in] xhp The pointer to the xbps_handle struct.
 * @param[in] pkgname Package name.
 * @param[in] version Package version.
 * @param[in] flush Set to true to make sure that pkgdb plist
 * is written to storage on success.
 *
 * @return 0 on success, otherwise an errno value.
 */
int xbps_unregister_pkg(struct xbps_handle *xhp,
			const char *pkgname,
			const char *version,
			bool flush);

/*@}*/

/** @addtogroup pkg_remove */
/*@{*/

/**
 * Remove an installed package.
 *
 * @param[in] xhp The pointer to the xbps_handle struct.
 * @param[in] pkgname Package name to match.
 * @param[in] version Package version associated.
 * @param[in] update If true, some steps will be skipped. See in the
 * detailed description above for more information.
 * @param[in] soft_replace If true, some steps will be skipped. See in
 * the detailed description above for more information.
 *
 * @return 0 on success, otherwise an errno value.
 */
int xbps_remove_pkg(struct xbps_handle *xhp,
		    const char *pkgname,
		    const char *version,
		    bool update,
		    bool soft_replace);

/**
 * Remove files defined in a proplib array as specified by \a key
 * of an installed package.
 * 
 * @param[in] xhp The pointer to the xbps_handle struct.
 * @param[in] dict Proplib dictionary internalized from package's
 * <b>XBPS_PKGFILES</b> definition in package's metadata directory.
 * The image in Detailed description shows off its structure.
 * @param[in] key Key of the array object to match, valid values are:
 * <b>files</b>, <b>dirs</b>, <b>links</b> and <b>conf_files</b>.
 * @param[in] pkgver Package/version string matching package dictionary,
 * i.e `foo-1.0'.
 *
 * @return 0 on success, otherwise an errno value.
 */
int xbps_remove_pkg_files(struct xbps_handle *xhp,
			  prop_dictionary_t dict,
			  const char *key,
			  const char *pkgver);

/*@}*/

/** @addtogroup transaction */
/*@{*/

/**
 * Finds a package by name or by pattern and enqueues it into
 * the transaction dictionary for future use. If pkg is a pkgname, the best
 * package version in repository pool will be queued, otherwise the first
 * repository matching the package pattern wins.
 *
 * @param[in] xhp Pointer to the xbps_handle struct.
 * @param[in] pkg Package name, package/version or package pattern to match, i.e
 * `foo', `foo-1.0' or `foo>=1.2'.
 * @param[in] reinstall If true, package will be queued (if \a str matches)
 * even if package is already installed.
 *
 * @return 0 on success, otherwise an errno value.
 * @retval EEXIST Package is already installed (reinstall wasn't enabled).
 * @retval ENOENT Package not matched in repository pool.
 * @retval ENOTSUP No repositories are available.
 * @retval EINVAL Any other error ocurred in the process.
 */
int xbps_transaction_install_pkg(struct xbps_handle *xhp,
				 const char *pkg,
				 bool reinstall);

/**
 * Marks a package as "going to be updated" in the transaction dictionary.
 * All repositories in the pool will be used, and newest version
 * available will be enqueued if it's greater than current installed
 * version.
 *
 * @param[in] xhp Pointer to the xbps_handle struct.
 * @param[in] pkgname The package name to update.
 *
 * @return 0 on success, otherwise an errno value.
 */
int xbps_transaction_update_pkg(struct xbps_handle *xhp, const char *pkgname);

/**
 * Finds newer versions for all installed packages by looking at the
 * repository pool. If a newer version exists, package will be enqueued
 * into the transaction dictionary.
 *
 * @param[in] xhp Pointer to the xbps_handle struct.
 * @return 0 on success, otherwise an errno value.
 */
int xbps_transaction_update_packages(struct xbps_handle *xhp);

/**
 * Removes a package currently installed. The package dictionary will
 * be added into the transaction dictionary.
 *
 * @param[in] xhp Pointer to the xbps_handle struct.
 * @param[in] pkgname Package name to be removed.
 * @param[in] recursive If true, all packages that are currently depending
 * on the package to be removed, and if they are orphans, will be added.
 *
 * @retval 0 success.
 * @retval ENOENT Package is not installed.
 * @retval EEXIST Package has reverse dependencies.
 * @retval EINVAL
 * @retval ENXIO A problem ocurred in the process.
 */
int xbps_transaction_remove_pkg(struct xbps_handle *xhp,
				const char *pkgname,
				bool recursive);

/**
 * Finds all package orphans currently installed and adds them into
 * the transaction dictionary.
 *
 * @param[in] xhp Pointer to the xbps_handle struct.
 *
 * @retval 0 success.
 * @retval ENOENT No package orphans were found.
 * @retval ENXIO
 * @retval EINVAL A problem ocurred in the process.
 */
int xbps_transaction_autoremove_pkgs(struct xbps_handle *xhp);

/**
 * Returns the transaction dictionary, as shown above in the image.
 * Before returning the package list is sorted in the correct order
 * and total installed/download size for the transaction is computed.
 *
 * @param[in] xhp Pointer to the xbps_handle struct.
 *
 * @retval 0 success.
 * @retval ENXIO if transaction dictionary and missing deps array were not created,
 *  due to xbps_transaction_install_pkg() or xbps_transaction_update_pkg() not
 *  previously called.
 * @retval ENODEV if there are missing dependencies in transaction ("missing_deps"
 *  array of strings object in xhp->transd dictionary).
 * @retval EAGAIN if there are package conflicts in transaction ("conflicts"
 *  array of strings object in xhp->transd dictionary).
 * @retval EINVAL There was an error sorting packages or computing the transaction
 * sizes.
 */
int xbps_transaction_prepare(struct xbps_handle *xhp);

/**
 * Commit a transaction. The transaction dictionary in xhp->transd contains all
 * steps to be executed in the transaction, as prepared by
 * xbps_transaction_prepare().
 *
 * @param[in] xhp Pointer to the xbps_handle struct.
 * @return 0 on success, otherwise an errno value.
 */
int xbps_transaction_commit(struct xbps_handle *xhp);

/*@}*/

/** @addtogroup plist_fetch */
/*@{*/

/**
 * Internalizes a plist file in a binary package file stored locally or
 * remotely as specified in the URL.
 *
 * @param[in] url URL to binary package file (full local or remote path).
 * @param[in] plistf Plist file name to internalize.
 *
 * @return An internalized proplib dictionary, otherwise NULL and
 * errno is set appropiately.
 */
prop_dictionary_t xbps_dictionary_metadata_plist_by_url(const char *url,
							const char *plistf);

/*@}*/

/** @addtogroup repopool */
/*@{*/

/**
 * @struct xbps_rpool_index xbps_api.h "xbps_api.h"
 * @brief Repository pool dictionary structure
 *
 * Repository index object structure registered in a private simple queue.
 * The structure contains a dictionary and the URI associated with the
 * registered repository index.
 */
struct xbps_rpool_index {
	/**
	 * @var repo
	 * 
	 * Internalized proplib array of the index plist file
	 * associated with repository.
	 */
	prop_array_t repo;
	/**
	 * @var uri
	 * 
	 * URI string associated with repository.
	 */
	const char *uri;
};

/**
 * Synchronizes the package index file for all remote repositories
 * as specified in the configuration file or if \a uri argument is
 * set, just sync the index file for that repository.
 *
 * @param[in] xhp Pointer to the xbps_handle struct.
 * @param[in] uri Repository URI to match for sync (optional).
 *
 * @return 0 on success, ENOTSUP if no repositories were found in
 * the configuration file.
 */
int xbps_rpool_sync(struct xbps_handle *xhp, const char *uri);

/**
 * Iterates over the repository pool and executes the \a fn function
 * callback passing in the void * \a arg argument to it. The bool pointer
 * argument can be used in the callbacks to stop immediately the loop if
 * set to true, otherwise it will only be stopped if it returns a
 * non-zero value.
 *
 * @param[in] xhp Pointer to the xbps_handle struct.
 * @param[in] fn Function callback to execute for every repository registered in
 * the pool.
 * @param[in] arg Opaque data passed in to the \a fn function callback for
 * client data.
 *
 * @return 0 on success, otherwise an errno value.
 */
int xbps_rpool_foreach(struct xbps_handle *xhp,
		       int (*fn)(struct xbps_handle *, struct xbps_rpool_index *, void *, bool *),
		       void *arg);

/**
 * Finds a package dictionary in the repository pool by specifying a
 * package pattern or a package name. This function does not take into
 * account virtual packages, just matches real packages.
 *
 * @param[in] xhp Pointer to the xbps_handle struct.
 * @param[in] pkg Package pattern or name.
 * @param[in] bypattern Set it to true if \a pkg is a pkgpattern (foo>=0),
 * false if it is a pkgname.
 * @param[in] best True to find the best version available in repo, false to
 * fetch the first package found matching \a pkg.
 *
 * @return The package dictionary if found, NULL otherwise.
 * @note When returned dictionary is no longer needed, you must release it
 * with prop_object_release(3).
 */
prop_dictionary_t xbps_rpool_find_pkg(struct xbps_handle *xhp,
				      const char *pkg,
				      bool bypattern,
				      bool best);

/**
 * Finds a package dictionary in repository pool by matching its \a pkgver
 * object.
 *
 * @param[in] xhp Pointer to the xbps_handle struct.
 * @param[in] pkgver Package name/version to match, i.e `foo-1.0'.
 *
 * @return The package dictionary if found, NULL otherwise.
 * @note When returned dictionary is no longer needed, you must release it
 * with prop_object_release(3).
 */
prop_dictionary_t xbps_rpool_find_pkg_exact(struct xbps_handle *xhp,
					    const char *pkgver);

/**
 * Finds a package dictionary in repository pool by specifying a
 * virtual package pattern or a package name.
 *
 * @param[in] xhp Pointer to the xbps_handle struct.
 * @param[in] pkg Virtual package pattern or name to match.
 * @param[in] bypattern Set it to true if \a pkg is a pkgpattern (foo>=0),
 * false if it is a pkgname.
 *
 * @return The package dictionary if found, NULL otherwise.
 * @note When returned dictionary is no longer needed, you must release it
 * with prop_object_release(3).
 */
prop_dictionary_t xbps_rpool_find_virtualpkg(struct xbps_handle *xhp,
					     const char *pkg,
					     bool bypattern);

/**
 * Finds a package dictionary in repository pool by specifying a
 * package pattern or a package name. Only virtual packages set in
 * configuration file will be matched.
 *
 * @param[in] xhp Pointer to the xbps_handle struct.
 * @param[in] pkg Virtual package pattern or name to match.
 * @param[in] bypattern Set it to true if \a pkg is a pkgpattern (foo>=0),
 * false if it is a pkgname.
 *
 * @return The package dictionary if found, NULL otherwise.
 * @note When returned dictionary is no longer needed, you must release it
 * with prop_object_release(3).
 */
prop_dictionary_t xbps_rpool_find_virtualpkg_conf(struct xbps_handle *xhp,
						  const char *pkg,
						  bool bypattern);

/**
 * Iterate over the the repository pool and search for a metadata plist
 * file in a binary package matching `pattern'. If a package is matched
 * the plist file \a plistf will be internalized into a proplib dictionary.
 *
 * When \a pattern is a pkgname, the newest package available in repositories
 * will be used. Otherwise the first repository matching \a pattern.
 *
 * @param[in] xhp Pointer to the xbps_handle struct.
 * @param[in] pattern Package name or package pattern to match, i.e `foo>=1.0'.
 * @param[in] plistf Plist file name to match, i.e XBPS_PKGPROPS or XBPS_PKGFILES.
 *
 * @return An internalized proplib dictionary of \a plistf, otherwise NULL
 * and errno is set appropiately.
 *
 * @note if NULL is returned and errno is ENOENT, that means that
 * binary package file has been found but the plist file could not
 * be found.
 */
prop_dictionary_t xbps_rpool_dictionary_metadata_plist(struct xbps_handle *xhp,
						       const char *pattern,
						       const char *plistf);

/*@}*/

/** @addtogroup reposync */
/*@{*/

/**
 * Syncs the package index file for a remote repository as specified
 * by the \a uri argument (if necessary).
 *
 * @param[in] xhp Pointer to the xbps_handle struct.
 * @param[in] uri URI to a remote repository.
 * @param[in] plistf Plist file to sync.
 *
 * @return -1 on error (errno is set appropiately), 0 if transfer was
 * not necessary (local/remote size/mtime matched) or 1 if
 * downloaded successfully.
 */
int xbps_repository_sync_pkg_index(struct xbps_handle *xhp,
				   const char *uri,
				   const char *plistf);

/*@}*/

/** @addtogroup pkgstates */
/*@{*/

/**
 * @enum pkg_state_t
 *
 * Integer representing a state on which a package may be. Possible
 * values for this are:
 *
 * - XBPS_PKG_STATE_HALF_UNPACKED: Package was being unpacked
 * but didn't finish properly.
 * - XBPS_PKG_STATE_UNPACKED: Package has been unpacked correctly
 * but has not been configured due to unknown reasons.
 * - XBPS_PKG_STATE_INSTALLED: Package has been installed successfully.
 * - XBPS_PKG_STATE_BROKEN: not yet used.
 * - XBPS_PKG_STATE_HALF_REMOVED: Package has been removed but not
 * completely: the purge action in REMOVE script wasn't executed, pkg
 * metadata directory still exists and is registered in package database.
 * - XBPS_PKG_STATE_NOT_INSTALLED: Package going to be installed in
 * a transaction dictionary but that has not been yet unpacked.
 */
typedef enum pkg_state {
	XBPS_PKG_STATE_UNPACKED = 1,
	XBPS_PKG_STATE_INSTALLED,
	XBPS_PKG_STATE_BROKEN,
	XBPS_PKG_STATE_HALF_REMOVED,
	XBPS_PKG_STATE_NOT_INSTALLED,
	XBPS_PKG_STATE_HALF_UNPACKED
} pkg_state_t;

/**
 * Gets package state from package \a pkgname, and sets its state
 * into \a state.
 * 
 * @param[in] xhp The pointer to an xbps_handle struct.
 * @param[in] pkgname Package name.
 * @param[out] state Package state returned.
 *
 * @return 0 on success, otherwise an errno value.
 */
int xbps_pkg_state_installed(struct xbps_handle *xhp,
			     const char *pkgname,
			     pkg_state_t *state);

/**
 * Gets package state from a package dictionary \a dict, and sets its
 * state into \a state.
 *
 * @param[in] dict Package dictionary.
 * @param[out] state Package state returned.
 *
 * @return 0 on success, otherwise an errno value.
 */
int xbps_pkg_state_dictionary(prop_dictionary_t dict, pkg_state_t *state);

/**
 * Sets package state \a state in package \a pkgname.
 *
 * @param[in] xhp The pointer to an xbps_handle struct.
 * @param[in] pkgname Package name.
 * @param[in] version Package version.
 * @param[in] state Package state to be set.
 *
 * @return 0 on success, otherwise an errno value.
 */
int xbps_set_pkg_state_installed(struct xbps_handle *xhp,
				 const char *pkgname,
				 const char *version,
				 pkg_state_t state);

/**
 * Sets package state \a state in package dictionary \a dict.
 *
 * @param[in] dict Package dictionary.
 * @param[in] state Package state to be set.
 *
 * @return 0 on success, otherwise an errno value.
 */
int xbps_set_pkg_state_dictionary(prop_dictionary_t dict, pkg_state_t state);

/*@}*/

/** @addtogroup util */
/*@{*/

/**
 * Creates a directory (and required components if necessary).
 *
 * @param[in] path Path for final directory.
 * @param[in] mode Mode for final directory (0755 if not specified).
 *
 * @return 0 on success, -1 on error and errno set appropiately.
 */
int xbps_mkpath(const char *path, mode_t mode);

/**
 * Returns a string by concatenating its variable argument list
 * as specified by the format string \a fmt.
 *
 * @param[in] fmt Format string, see printf(3).
 * @return A pointer to a malloc(3)ed string, NULL otherwise and errno
 * is set appropiately. The pointer should be free(3)d when it's
 * no longer needed.
 */
char *xbps_xasprintf(const char *fmt, ...);

/**
 * Returns a string with the sha256 hash for the file specified
 * by \a file.
 *
 * @param[in] file Path to a file.
 * @return A pointer to a malloc(3)ed string, NULL otherwise and errno
 * is set appropiately. The pointer should be free(3)d when it's no
 * longer needed.
 */
char *xbps_file_hash(const char *file);

/**
 * Returns a string with the sha256 hash for the file specified
 * by \a file in an array with key \a key in the proplib dictionary
 * \a d.
 *
 * @param[in] d Proplib dictionary to look in.
 * @param[in] key Array key to match in dictionary.
 * @param[in] file Pathname to a file.
 *
 * @return The sha256 hash string if found, NULL otherwise
 * and errno is set appropiately.
 */
const char *xbps_file_hash_dictionary(prop_dictionary_t d,
				      const char *key,
				      const char *file);

/**
 * Compares the sha256 hash of the file \a file with the sha256
 * string specified by \a sha256.
 *
 * @param[in] file Path to a file.
 * @param[in] sha256 SHA256 hash to compare.
 *
 * @return 0 if \a file and \a sha256 have the same hash, ERANGE
 * if it differs, or any other errno value on error.
 */
int xbps_file_hash_check(const char *file, const char *sha256);

/**
 * Checks if \a file matches the sha256 hash specified in the array
 * with key \a key in the proplib dictionary \a d.
 *
 * @param[in] xhp The pointer to an xbps_handle struct.
 * @param[in] d Proplib dictionary to look in.
 * @param[in] key Proplib array key to match for file.
 * @param[in] file Pathname to a file.
 *
 * @return 0 if hash is matched, -1 on error and 1 if no match.
 */
int xbps_file_hash_check_dictionary(struct xbps_handle *xhp,
				    prop_dictionary_t d,
				    const char *key,
				    const char *file);

/**
 * Checks if a package is currently installed by matching a package
 * pattern string.
 *
 * @param[in] xhp The pointer to an xbps_handle struct.
 * @param[in] pkg Package pattern used to find the package.
 *
 * @return -1 on error (errno set appropiately), 0 if package pattern
 * didn't match installed package, 1 if \a pkg pattern fully
 * matched installed package.
 */
int xbps_check_is_installed_pkg_by_pattern(struct xbps_handle *xhp,
					   const char *pkg);

/**
 * Checks if package \a pkgname is currently installed.
 *
 * @param[in] xhp The pointer to an xbps_handle struct.
 * @param[in] pkgname Package name.
 *
 * @return True if \a pkgname is installed, false otherwise.
 */
bool xbps_check_is_installed_pkg_by_name(struct xbps_handle *xhp,
					 const char *pkgname);

/**
 * Checks if the URI specified by \a uri is remote or local.
 *
 * @param[in] uri URI string.
 * 
 * @return true if URI is remote, false if local.
 */
bool xbps_check_is_repository_uri_remote(const char *uri);

/**
 * Gets the full URI to a binary package file as returned by a
 * package dictionary from a repository in \a pkgd, by looking at the
 * repository location object "repository" in its dictionary.
 *
 * @param[in] xhp The pointer to an xbps_handle struct.
 * @param[in] pkgd Package dictionary stored in a transaction dictionary.
 * @param[in] repoloc Repository URL location string.
 *
 * @return A pointer to a malloc(3)ed string, NULL otherwise and
 * errno is set appropiately. The pointer should be free(3)d when it's
 * no longer needed.
 */ 
char *xbps_path_from_repository_uri(struct xbps_handle *xhp,
				    prop_dictionary_t pkgd,
				    const char *repoloc);

/**
 * Gets the full path to a repository package index plist file, as
 * specified by \a uri.
 *
 * @param[in] xhp The pointer to an xbps_handle struct.
 * @param[in] uri Repository URI.
 *
 * @return A pointer to a malloc(3)d string, NULL otherwise and
 * errno is set appropiately. The pointer should be free(3)d when it's
 * no longer needed.
 */
char *xbps_pkg_index_plist(struct xbps_handle *xhp, const char *uri);

/**
 * Returns the full path to a repository package index files plist file,
 * as specified by \a uri.
 *
 * @param[in] xhp The pointer to an xbps_handle struct.
 * @param[in] uri Repository URI.
 *
 * @return A pointer to a malloc(3)ed string, NULL otherwise and
 * errno is set appropiately. The pointer should be free(3)d when it's
 * no longer needded.
 */
char *xbps_pkg_index_files_plist(struct xbps_handle *xhp, const char *uri);

/**
 * Gets the name of a package string. Package strings are composed
 * by a @<pkgname@>/@<version@> pair and separated by the <em>minus</em>
 * sign, i.e <b>foo-2.0</b>.
 *
 * @param[in] pkg Package string.
 *
 * @return A pointer to a malloc(3)d string, NULL otherwise and
 * errno is set appropiately. The pointer should be free(3)d when it's
 * no longer needed.
 */
char *xbps_pkg_name(const char *pkg);

/**
 * Gets a the package name of a package pattern string specified by
 * the \a pattern argument.
 *
 * @param[in] pattern A package pattern. Package patterns are composed
 * by looking at <b>'><='</b> to split components, i.e <b>foo>=2.0</b>,
 * <b>blah<1.0</b>, <b>blob==2.0</b>, etc.
 *
 * @return A pointer to a malloc(3)ed string with the package name,
 * NULL otherwise and errno is set appropiately. The pointer should be
 * free(3)d when it's no longer needed.
 */
char *xbps_pkgpattern_name(const char *pattern);

/**
 * Gets the package version in a package string, i.e <b>foo-2.0</b>.
 * 
 * @param[in] pkg Package string.
 *
 * @return A string with the version string, NULL if it couldn't
 * find the version component.
 */
const char *xbps_pkg_version(const char *pkg);

/**
 * Gets the package version of a package pattern string specified by
 * the \a pattern argument.
 *
 * @param[in] pattern A package pattern. The same rules in
 * xbps_get_pkgpattern_name() apply here.
 *
 * @return A string with the pattern version, NULL otherwise and
 * errno is set appropiately.
 */
const char *xbps_pkgpattern_version(const char *pattern);

/**
 * Package pattern matching.
 *
 * @param[in] pkgver Package name/version, i.e `foo-1.0'.
 * @param[in] pattern Package pattern to match against \a pkgver.
 * There are 3 strategies for version matching:
 *  - simple compare: pattern equals to pkgver.
 *  - shell wildcards: see fnmatch(3).
 *  - relational dewey matching: '>' '<' '>=' '<='.
 *
 * @return 1 if \a pkgver is matched against \a pattern, 0 if no match.
 */
int xbps_pkgpattern_match(const char *pkgver, const char *pattern);

/**
 * Gets the package version revision in a package string.
 *
 * @param[in] pkg Package string, i.e <b>foo-2.0_1</b>.
 *
 * @return A string with the revision number, NULL if it couldn't
 * find the revision component.
 */
const char *xbps_pkg_revision(const char *pkg);

/**
 * Checks if a package has run dependencies.
 *
 * @param[in] dict Package dictionary.
 *
 * @return True if package has run dependencies, false otherwise.
 */
bool xbps_pkg_has_rundeps(prop_dictionary_t dict);

/**
 * Returns true if provided string is valid for target architecture.
 *
 * @param[in] xhp The pointer to an xbps_handle struct.
 * @param[in] orig Architecture to match.
 * @param[in] target If not NULL, \a orig will be matched against it
 * rather than returned value of uname(2).
 *
 * @return True on match, false otherwise.
 */
bool xbps_pkg_arch_match(struct xbps_handle *xhp,
			 const char *orig,
			 const char *target);

/**
 * Converts the 64 bits signed number specified in \a bytes to
 * a human parsable string buffer pointed to \a buf.
 *
 * @param[out] buf Buffer to store the resulting string. At least
 * it should have space for 6 chars.
 * @param[in] bytes 64 bits signed number to convert.
 *
 * @return A negative number is returned on error, 0 otherwise.
 */
int xbps_humanize_number(char *buf, int64_t bytes);

/**
 * Compares package version strings.
 *
 * The package version is defined by:
 * ${VERSION}[_${REVISION}].
 *
 * @param[in] pkg1 a package version string.
 * @param[in] pkg2 a package version string.
 *
 * @return -1, 0 or 1 depending if pkg1 is less than, equal to or
 * greater than pkg2.
 */
int xbps_cmpver(const char *pkg1, const char *pkg2);

/*@}*/

__END_DECLS

#endif /* !_XBPS_API_H_ */
