/*-
 * Copyright (c) 2012 Juan Romero Pardines.
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
#include <string.h>
#include <atf-c.h>
#include <xbps_api.h>

static const char arrayxml[] =
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
"<plist version=\"1.0\">\n"
"<array>\n"
"	<dict>\n"
"		<key>pkgname</key>\n"
"		<string>afoo</string>\n"
"		<key>version</key>\n"
"		<string>1.1</string>\n"
"		<key>pkgver</key>\n"
"		<string>afoo-1.1</string>\n"
"		<key>provides</key>\n"
"		<array>\n"
"			<string>virtualpkg-9999</string>\n"
"		</array>\n"
"	</dict>\n"
"	<dict>\n"
"		<key>pkgname</key>\n"
"		<string>foo</string>\n"
"		<key>version</key>\n"
"		<string>2.0</string>\n"
"		<key>pkgver</key>\n"
"		<string>foo-2.0</string>\n"
"	</dict>\n"
"</array>\n"
"</plist>\n";

ATF_TC(find_pkg_in_array_by_name_test);
ATF_TC_HEAD(find_pkg_in_array_by_name_test, tc)
{
	atf_tc_set_md_var(tc, "descr", "Test xbps_find_pkg_in_array_by_name");
}
ATF_TC_BODY(find_pkg_in_array_by_name_test, tc)
{
	struct xbps_handle xh;
	prop_array_t a;
	prop_dictionary_t dr;

	a = prop_array_internalize(arrayxml);
	ATF_REQUIRE_EQ(prop_object_type(a), PROP_TYPE_ARRAY);

	memset(&xh, 0, sizeof(xh));
	xbps_init(&xh);
	/* match by pkgname */
	dr = xbps_find_pkg_in_array_by_name(&xh, a, "foo", NULL);
	ATF_REQUIRE_EQ(prop_object_type(dr), PROP_TYPE_DICTIONARY);
}

ATF_TC(find_pkg_in_array_by_pattern_test);
ATF_TC_HEAD(find_pkg_in_array_by_pattern_test, tc)
{
	atf_tc_set_md_var(tc, "descr", "Test xbps_find_pkg_in_array_by_pattern");
}
ATF_TC_BODY(find_pkg_in_array_by_pattern_test, tc)
{
	struct xbps_handle xh;
	prop_array_t a;
	prop_dictionary_t dr;

	a = prop_array_internalize(arrayxml);
	ATF_REQUIRE_EQ(prop_object_type(a), PROP_TYPE_ARRAY);

	memset(&xh, 0, sizeof(xh));
	xbps_init(&xh);
	/* match by pkgpattern */
	dr = xbps_find_pkg_in_array_by_pattern(&xh, a, "foo>=2.0", NULL);
	ATF_REQUIRE_EQ(prop_object_type(dr), PROP_TYPE_DICTIONARY);
}

ATF_TC(find_pkg_in_array_by_pkgver_test);
ATF_TC_HEAD(find_pkg_in_array_by_pkgver_test, tc)
{
	atf_tc_set_md_var(tc, "descr", "Test xbps_find_pkg_in_array_by_pkgver");
}
ATF_TC_BODY(find_pkg_in_array_by_pkgver_test, tc)
{
	struct xbps_handle xh;
	prop_array_t a;
	prop_dictionary_t dr;

	a = prop_array_internalize(arrayxml);
	ATF_REQUIRE_EQ(prop_object_type(a), PROP_TYPE_ARRAY);

	memset(&xh, 0, sizeof(xh));
	xbps_init(&xh);
	/* exact match by pkgver */
	dr = xbps_find_pkg_in_array_by_pkgver(&xh, a, "foo-2.0", NULL);
	ATF_REQUIRE_EQ(prop_object_type(dr), PROP_TYPE_DICTIONARY);
}

ATF_TC(find_virtualpkg_in_array_by_name_test);
ATF_TC_HEAD(find_virtualpkg_in_array_by_name_test, tc)
{
	atf_tc_set_md_var(tc, "descr", "Test xbps_find_virtualpkg_in_array_by_name");
}
ATF_TC_BODY(find_virtualpkg_in_array_by_name_test, tc)
{
	struct xbps_handle xh;
	prop_array_t a;
	prop_dictionary_t dr;
	const char *pkgname;

	a = prop_array_internalize(arrayxml);
	ATF_REQUIRE_EQ(prop_object_type(a), PROP_TYPE_ARRAY);

	memset(&xh, 0, sizeof(xh));
	xbps_init(&xh);
	dr = xbps_find_virtualpkg_in_array_by_name(&xh, a, "virtualpkg");
	ATF_REQUIRE_EQ(prop_object_type(dr), PROP_TYPE_DICTIONARY);
	prop_dictionary_get_cstring_nocopy(dr, "pkgname", &pkgname);
	ATF_REQUIRE_STREQ(pkgname, "afoo");
}

ATF_TC(find_virtualpkg_in_array_by_pattern_test);
ATF_TC_HEAD(find_virtualpkg_in_array_by_pattern_test, tc)
{
	atf_tc_set_md_var(tc, "descr", "Test xbps_find_virtualpkg_in_array_by_pattern");
}
ATF_TC_BODY(find_virtualpkg_in_array_by_pattern_test, tc)
{
	struct xbps_handle xh;
	prop_array_t a;
	prop_dictionary_t dr;
	const char *pkgname;

	a = prop_array_internalize(arrayxml);
	ATF_REQUIRE_EQ(prop_object_type(a), PROP_TYPE_ARRAY);

	memset(&xh, 0, sizeof(xh));
	xbps_init(&xh);
	dr = xbps_find_virtualpkg_in_array_by_pattern(&xh, a, "virtualpkg>=9999");
	ATF_REQUIRE_EQ(prop_object_type(dr), PROP_TYPE_DICTIONARY);
	prop_dictionary_get_cstring_nocopy(dr, "pkgname", &pkgname);
	ATF_REQUIRE_STREQ(pkgname, "afoo");
}

ATF_TP_ADD_TCS(tp)
{
	ATF_TP_ADD_TC(tp, find_pkg_in_array_by_name_test);
	ATF_TP_ADD_TC(tp, find_pkg_in_array_by_pattern_test);
	ATF_TP_ADD_TC(tp, find_pkg_in_array_by_pkgver_test);
	ATF_TP_ADD_TC(tp, find_virtualpkg_in_array_by_name_test);
	ATF_TP_ADD_TC(tp, find_virtualpkg_in_array_by_pattern_test);

	return atf_no_error();
}
