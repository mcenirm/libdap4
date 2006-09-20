// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

#include "config.h"

static char rcsid[] not_used = {"$Id$"};

#include "ConstraintEvaluator.h"

#include "ce_functions.h"
#include "parser.h"
#include "ce_parser.h"
#include "debug.h"

using namespace libdap;

struct yy_buffer_state;
yy_buffer_state *ce_expr_scan_string(const char *str);
int ce_exprparse(void *arg);

// Glue routines declared in expr.lex
void ce_expr_switch_to_buffer(void *new_buffer);
void ce_expr_delete_buffer(void * buffer);
void *ce_expr_string(const char *yy_str);

ConstraintEvaluator::ConstraintEvaluator()
{
    register_functions(*this);
}

ConstraintEvaluator::~ConstraintEvaluator()
{
    // delete all the constants created by the parser for CE evaluation
    for (Constants_iter j = constants.begin(); j != constants.end(); j++) {
        BaseType *btp = *j ;
        delete btp ; btp = 0;
    }

    for (Clause_iter k = expr.begin(); k != expr.end(); k++) {
        Clause *cp = *k ;
        delete cp ; cp = 0;
    }
}

/** Returns the first clause in a parsed constraint expression. */
ConstraintEvaluator::Clause_iter
ConstraintEvaluator::clause_begin()
{
    return expr.begin() ;
}

/** Returns a reference to the end of the list of clauses in a parsed 
    constraint expression. It does not reference the last clause */
ConstraintEvaluator::Clause_iter
ConstraintEvaluator::clause_end()
{
    return expr.end() ;
}

/** Returns the value of the indicated clause of a constraint
    expression. */
bool
ConstraintEvaluator::clause_value(Clause_iter &iter, DDS &dds,
                                  const string &dataset)
{
    if(expr.empty())
        throw InternalErr(__FILE__, __LINE__, 
                          "There are no CE clauses for *this* DDS object.");

    return (*iter)->value(dataset, dds);
}

/** @brief Add a clause to a constraint expression.

    This function adds an operator clause to the constraint
    expression. 

    @param op An integer indicating the operator in use.  These
    values are generated by \c bison.
    @param arg1 A pointer to the argument on the left side of the
    operator. 
    @param arg2 A pointer to a list of the arguments on the right
    side of the operator.
*/
void
ConstraintEvaluator::append_clause(int op, rvalue *arg1, rvalue_list *arg2)
{
    Clause *clause = new Clause(op, arg1, arg2);

    expr.push_back(clause);
}

/** @brief Add a clause to a constraint expression.

    This function adds a boolean function clause to the constraint
    expression. 

    @param func A pointer to a boolean function from the list of
    supported functions.
    @param args A list of arguments to that function.
*/
void
ConstraintEvaluator::append_clause(bool_func func, rvalue_list *args)
{
    Clause *clause = new Clause(func, args);

    expr.push_back(clause);
}

/** @brief Add a clause to a constraint expression.

    This function adds a real-valued (BaseType) function clause to
    the constraint expression.

    @param func A pointer to a BaseType function from the list of
    supported functions.
    @param args A list of arguments to that function.
*/
void
ConstraintEvaluator::append_clause(btp_func func, rvalue_list *args)
{
    Clause *clause = new Clause(func, args);

    expr.push_back(clause);
}

/** The DDS maintains a list of BaseType pointers for all the constants
    that the constraint expression parser generates. These objects are
    deleted by the DDS destructor. Note that there are no list accessors;
    these constants are never accessed from the list. The list is simply
    a convenient way to make sure the constants are disposed of properly.

    the constraint expression parser. */
void
ConstraintEvaluator::append_constant(BaseType *btp)
{
    constants.push_back(btp);
}

/** Each DDS carries with it a list of external functions it can use to
    evaluate a constraint expression. If a constraint contains any of
    these functions, the entries in the list allow the parser to evaluate
    it. The functions are of two types: those that return boolean values,
    and those that return real (also called BaseType) values.

    These methods are used to manipulate this list of known
    external functions.

    @name External Function Accessors
*/
//@{

/** @brief Add a boolean function to the list. */
void
ConstraintEvaluator::add_function(const string &name, bool_func f)
{
    function func(name, f);
    functions.push_back(func);
}

/** @brief Add a BaseType function to the list. */
void
ConstraintEvaluator::add_function(const string &name, btp_func f)
{
    function func(name, f);
    functions.push_back(func);
}

/** @brief Add a projection function to the list. */
void
ConstraintEvaluator::add_function(const string &name, proj_func f)
{
    function func(name, f);
    functions.push_back(func);
}

/** @brief Find a Boolean function with a given name in the function list. */
bool
ConstraintEvaluator::find_function(const string &name, bool_func *f) const
{
    if (functions.empty())
        return false;

    for (Functions_citer i = functions.begin(); i != functions.end(); i++)
        {
            if (name == (*i).name && (*f = (*i).b_func)) {
                return true;
            }
        }

    return false;
}

/** @brief Find a BaseType function with a given name in the function list. */
bool
ConstraintEvaluator::find_function(const string &name, btp_func *f) const
{
    if (functions.empty())
        return false;

    for (Functions_citer i = functions.begin(); i != functions.end(); i++)
        {
            if (name == (*i).name && (*f = (*i).bt_func)) {
                return true;
            }
        }

    return false;
}

/** @brief Find a projection function with a given name in the function list. */
bool
ConstraintEvaluator::find_function(const string &name, proj_func *f) const
{
    if (functions.empty())
        return false;

    for (Functions_citer i = functions.begin(); i != functions.end(); i++)
        if (name == (*i).name && (*f = (*i).p_func)) {
            return true;
        }

    return false;
}
//@}

/** @brief Does the current constraint expression return a BaseType
    pointer? */
bool
ConstraintEvaluator::functional_expression()
{
    if (expr.empty())
        return false;

    Clause *cp = expr[0] ;
    return cp->value_clause();
}

/** @brief Evaluate a function-valued constraint expression. */
BaseType *
ConstraintEvaluator::eval_function(DDS &dds, const string &dataset)
{
    if (expr.size() != 1)
        throw InternalErr(__FILE__, __LINE__, 
                          "The length of the list of CE clauses is not 1.");

    Clause *cp = expr[0] ;
    BaseType *result;
    if (cp->value(dataset, dds, &result))
        return result;
    else
        return NULL;
}

/** @brief Does the current constraint expression return a boolean value? */ 
bool
ConstraintEvaluator::boolean_expression()
{
    if (expr.empty())
        return false;

    bool boolean = true;
    for (Clause_iter i = expr.begin(); i != expr.end(); i++)
    {
        boolean = boolean && (*i)->boolean_clause();
    }
    
    return boolean;
}


/** @brief Evaluate a boolean-valued constraint expression. */
bool
ConstraintEvaluator::eval_selection(DDS &dds, const string &dataset)
{
    if (expr.empty()) {
        DBG(cerr << "No selection recorded" << endl);
        return true;
    }

    DBG(cerr << "Eval selection" << endl);

    // A CE is made up of zero or more clauses, each of which has a boolean
    // value. The value of the CE is the logical AND of the clause
    // values. See ConstraintEvaluator::clause::value(...) for inforamtion on logical ORs in
    // CEs. 
    bool result = true;
    for (Clause_iter i = expr.begin(); i != expr.end() && result; i++)
    {
        // A selection expression *must* contain only boolean clauses!
        if(!((*i)->boolean_clause()))
            throw InternalErr(__FILE__, __LINE__, 
                "A selection expression must contain only boolean clauses.");
        result = result && (*i)->value(dataset, dds);
    }

    return result;
}

/** @brief Parse the constraint expression given the current DDS. 

    Parse the constraint expression.
    As a side effect, mark the DDS so that BaseType's mfuncs can be used to
    correctly read the variable's value and send it to the client.

    @param constraint A string containing the constraint expression.
    @param dds The DDS that provides the environment within which the 
    constraint is evaluated.
    @exception Throws Error if the constraint does not parse. */
void
ConstraintEvaluator::parse_constraint(const string &constraint, DDS &dds)
{
    void *buffer = ce_expr_string(constraint.c_str());
    ce_expr_switch_to_buffer(buffer);

    ce_parser_arg arg(this, &dds);

    // For all errors, exprparse will throw Error. 
    ce_exprparse((void *)&arg);

    ce_expr_delete_buffer(buffer);
}
