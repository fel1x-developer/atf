//
// Automated Testing Framework (atf)
//
// Copyright (c) 2007 The NetBSD Foundation, Inc.
// All rights reserved.
//
// This code is derived from software contributed to The NetBSD Foundation
// by Julio M. Merino Vidal, developed as part of Google's Summer of Code
// 2007 program.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this
//    software must display the following acknowledgement:
//        This product includes software developed by the NetBSD
//        Foundation, Inc. and its contributors.
// 4. Neither the name of The NetBSD Foundation nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND
// CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
// IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
// IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include <cstdlib>

#include <atf.hpp>

#include "atfprivate/config.hpp"

namespace std {
#if !defined(HAVE_PUTENV_IN_STD)
    using ::putenv;
#endif
#if !defined(HAVE_SETENV_IN_STD)
    using ::setenv;
#endif
#if !defined(HAVE_UNSETENV_IN_STD)
    using ::unsetenv;
#endif
}

namespace atf {
    namespace config {
        void __reinit(void);
    }
}

static
void
set_env_var(const char* name, const char* val)
{
#if defined(HAVE_SETENV)
    if (std::setenv(name, val, 1) == -1)
        ATF_FAIL(std::string("set_env_var(") + name + ") failed");
#elif defined(HAVE_PUTENV)
    if (std::putenv(std::string(name) + "=" + val) == -1)
        ATF_FAIL(std::string("set_env_var(") + name + ") failed");
#else
#   error "Don't know how to set an environment variable."
#endif
}

static
void
unset_env_var(const char* name)
{
#if defined(HAVE_UNSETENV)
    std::unsetenv(name);
#elif defined(HAVE_PUTENV)
    if (std::putenv(std::string(name) + "=") == -1)
        ATF_FAIL(std::string("unset_env_var(") + name + ") failed");
#else
#   error "Don't know how to unset an environment variable."
#endif
}

ATF_TEST_CASE(get);
ATF_TEST_CASE_HEAD(get)
{
    set("descr", "Tests the config::get function");
}
ATF_TEST_CASE_BODY(get)
{
    // Unset all known environment variables and make sure the built-in
    // values do not match the bogus value we will use for testing.
    unset_env_var("ATF_LIBEXECDIR");
    unset_env_var("ATF_PKGDATADIR");
    unset_env_var("ATF_SHELL");
    atf::config::__reinit();
    ATF_CHECK(atf::config::get("atf_libexecdir") != "env-value");
    ATF_CHECK(atf::config::get("atf_pkgdatadir") != "env-value");
    ATF_CHECK(atf::config::get("atf_shell") != "env-value");

    // Check if the ATF_LIBEXECDIR variable is recognized.
    set_env_var  ("ATF_LIBEXECDIR", "env-value");
    unset_env_var("ATF_PKGDATADIR");
    unset_env_var("ATF_SHELL");
    atf::config::__reinit();
    ATF_CHECK_EQUAL(atf::config::get("atf_libexecdir"), "env-value");
    ATF_CHECK(atf::config::get("atf_pkgdatadir") != "env-value");
    ATF_CHECK(atf::config::get("atf_shell") != "env-value");

    // Check if the ATF_PKGDATADIR variable is recognized.
    unset_env_var("ATF_LIBEXECDIR");
    set_env_var  ("ATF_PKGDATADIR", "env-value");
    unset_env_var("ATF_SHELL");
    atf::config::__reinit();
    ATF_CHECK(atf::config::get("atf_libexecdir") != "env-value");
    ATF_CHECK_EQUAL(atf::config::get("atf_pkgdatadir"), "env-value");
    ATF_CHECK(atf::config::get("atf_shell") != "env-value");

    // Check if the ATF_SHELL variable is recognized.
    unset_env_var("ATF_LIBEXECDIR");
    unset_env_var("ATF_PKGDATADIR");
    set_env_var  ("ATF_SHELL", "env-value");
    atf::config::__reinit();
    ATF_CHECK(atf::config::get("atf_libexecdir") != "env-value");
    ATF_CHECK(atf::config::get("atf_pkgdatadir") != "env-value");
    ATF_CHECK_EQUAL(atf::config::get("atf_shell"), "env-value");
}

ATF_TEST_CASE(get_all);
ATF_TEST_CASE_HEAD(get_all)
{
    set("descr", "Tests the config::get_all function");
}
ATF_TEST_CASE_BODY(get_all)
{
    atf::config::__reinit();

    // Check that the valid variables, and only those, are returned.
    std::map< std::string, std::string > vars = atf::config::get_all();
    ATF_CHECK_EQUAL(vars.size(), 3);
    ATF_CHECK(vars.find("atf_libexecdir") != vars.end());
    ATF_CHECK(vars.find("atf_pkgdatadir") != vars.end());
    ATF_CHECK(vars.find("atf_shell") != vars.end());
}

ATF_TEST_CASE(has);
ATF_TEST_CASE_HEAD(has)
{
    set("descr", "Tests the config::has function");
}
ATF_TEST_CASE_BODY(has)
{
    atf::config::__reinit();

    // Check for all the variables that must exist.
    ATF_CHECK(atf::config::has("atf_libexecdir"));
    ATF_CHECK(atf::config::has("atf_pkgdatadir"));
    ATF_CHECK(atf::config::has("atf_shell"));

    // Same as above, but using uppercase (which is incorrect).
    ATF_CHECK(!atf::config::has("ATF_LIBEXECDIR"));
    ATF_CHECK(!atf::config::has("ATF_PKGDATADIR"));
    ATF_CHECK(!atf::config::has("ATF_SHELL"));

    // Check for some other variables that cannot exist.
    ATF_CHECK(!atf::config::has("foo"));
    ATF_CHECK(!atf::config::has("BAR"));
    ATF_CHECK(!atf::config::has("atf_foo"));
    ATF_CHECK(!atf::config::has("ATF_BAR"));
    ATF_CHECK(!atf::config::has("atf_shel"));
    ATF_CHECK(!atf::config::has("atf_shells"));
}

ATF_INIT_TEST_CASES(tcs)
{
    tcs.push_back(&has);
    tcs.push_back(&get);
    tcs.push_back(&get_all);
}