
// Copyright Aleksey Gurtovoy 2000-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Source: /CVSROOT/boost/libs/mpl/test/iterator_tags.cpp,v $
// $Date: 2007/10/29 07:32:56 $
// $Revision: 1.1.1.1 $

#include <boost/mpl/iterator_tags.hpp>

#include <boost/mpl/less.hpp>
#include <boost/mpl/aux_/test.hpp>

MPL_TEST_CASE()
{
    MPL_ASSERT(( less<mpl::forward_iterator_tag,mpl::bidirectional_iterator_tag> ));
    MPL_ASSERT(( less<mpl::bidirectional_iterator_tag,mpl::random_access_iterator_tag> ));
}
