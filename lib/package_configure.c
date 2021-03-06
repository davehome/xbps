/*-
 * Copyright (c) 2009-2012 Juan Romero Pardines.
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
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "xbps_api_impl.h"

/**
 * @file lib/package_configure.c
 * @brief Package configuration routines
 * @defgroup configure Package configuration functions
 *
 * Configure a package or all packages. Only packages in XBPS_PKG_STATE_UNPACKED
 * state will be processed (unless overriden). Package configuration steps:
 *  - Its <b>post-install</b> target in the INSTALL script will be executed.
 *  - Its state will be changed to XBPS_PKG_STATE_INSTALLED if previous step
 *    ran successful.
 *
 * @note
 * If the \a XBPS_FLAG_FORCE_CONFIGURE is set through xbps_init() in the flags
 * member, the package (or packages) will be reconfigured even if its
 * state is XBPS_PKG_STATE_INSTALLED.
 */
int
xbps_configure_packages(struct xbps_handle *xhp, bool flush)
{
	prop_object_t obj;
	const char *pkgname;
	size_t i;
	int rv;

	if ((rv = xbps_pkgdb_init(xhp)) != 0)
		return rv;

	for (i = 0; i < prop_array_count(xhp->pkgdb); i++) {
		obj = prop_array_get(xhp->pkgdb, i);
		prop_dictionary_get_cstring_nocopy(obj, "pkgname", &pkgname);
		rv = xbps_configure_pkg(xhp, pkgname, true, false, false);
		if (rv != 0)
			break;
	}
	if (flush)
		rv = xbps_pkgdb_update(xhp, true);

	return rv;
}

int
xbps_configure_pkg(struct xbps_handle *xhp,
		   const char *pkgname,
		   bool check_state,
		   bool update,
		   bool flush)
{
	prop_dictionary_t pkgd;
	const char *version, *pkgver;
	char *buf;
	int rv = 0;
	pkg_state_t state = 0;

	assert(pkgname != NULL);

	pkgd = xbps_pkgdb_get_pkgd(xhp, pkgname, false);
	if (pkgd == NULL)
		return ENOENT;

	prop_dictionary_get_cstring_nocopy(pkgd, "version", &version);
	prop_dictionary_get_cstring_nocopy(pkgd, "pkgver", &pkgver);

	if (check_state) {
		rv = xbps_pkg_state_dictionary(pkgd, &state);
		xbps_dbg_printf(xhp, "%s: state %d rv %d\n", pkgname, state, rv);
		if (rv != 0) {
			xbps_dbg_printf(xhp, "%s: [configure] failed to get "
			    "pkg state: %s\n", pkgname, strerror(rv));
			prop_object_release(pkgd);
			return EINVAL;
		}

		if (state == XBPS_PKG_STATE_INSTALLED) {
			if ((xhp->flags & XBPS_FLAG_FORCE_CONFIGURE) == 0)
				return 0;
		} else if (state != XBPS_PKG_STATE_UNPACKED)
			return EINVAL;
	}
	xbps_set_cb_state(xhp, XBPS_STATE_CONFIGURE, 0, pkgname, version, NULL);

	buf = xbps_xasprintf("%s/metadata/%s/INSTALL",
	    XBPS_META_PATH, pkgname);
	if (buf == NULL)
		return ENOMEM;

	if (chdir(xhp->rootdir) == -1) {
		xbps_set_cb_state(xhp, XBPS_STATE_CONFIGURE_FAIL,
		    errno, pkgname, version,
		    "%s: [configure] failed to chdir to rootdir `%s': %s",
		    pkgver, xhp->rootdir, strerror(errno));
		free(buf);
		return EINVAL;
	}

	if (access(buf, X_OK) == 0) {
		if (xbps_file_exec(xhp, buf, "post",
		    pkgname, version, update ? "yes" : "no",
		    xhp->conffile, NULL) != 0) {
			xbps_set_cb_state(xhp, XBPS_STATE_CONFIGURE_FAIL,
			    errno, pkgname, version,
			    "%s: [configure] INSTALL script failed to execute "
			    "the post ACTION: %s", pkgver, strerror(errno));
			free(buf);
			return errno;
		}
	} else {
		if (errno != ENOENT) {
			xbps_set_cb_state(xhp, XBPS_STATE_CONFIGURE_FAIL,
			    errno, pkgname, version,
			    "%s: [configure] INSTALL script cannot be "
			    "executed: %s", pkgver, strerror(errno));
			free(buf);
			return errno;
		}
	}
	free(buf);
	rv = xbps_set_pkg_state_installed(xhp, pkgname, version,
	    XBPS_PKG_STATE_INSTALLED);
	if (rv != 0) {
		xbps_set_cb_state(xhp, XBPS_STATE_CONFIGURE_FAIL, rv,
		    pkgname, version,
		    "%s: [configure] failed to set state to installed: %s",
		    pkgver, strerror(rv));
	}
	if (flush) {
		if ((rv = xbps_pkgdb_update(xhp, true)) != 0) {
			xbps_set_cb_state(xhp, XBPS_STATE_CONFIGURE_FAIL, rv,
			    pkgname, version,
			    "%s: [configure] failed to update pkgdb: %s\n",
			    pkgver, strerror(rv));
		}
	}
	return rv;
}
