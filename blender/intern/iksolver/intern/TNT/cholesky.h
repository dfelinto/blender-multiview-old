/**
 * $Id$
 * ***** BEGIN GPL/BL DUAL LICENSE BLOCK *****
 *
 * The contents of this file may be used under the terms of either the GNU
 * General Public License Version 2 or later (the "GPL", see
 * http://www.gnu.org/licenses/gpl.html ), or the Blender License 1.0 or
 * later (the "BL", see http://www.blender.org/BL/ ) which has to be
 * bought from the Blender Foundation to become active, in which case the
 * above mentioned GPL option does not apply.
 *
 * The Original Code is Copyright (C) 2002 by NaN Holding BV.
 * All rights reserved.
 *
 * The Original Code is: all of this file.
 *
 * Contributor(s): none yet.
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
 */

/*

*
* Template Numerical Toolkit (TNT): Linear Algebra Module
*
* Mathematical and Computational Sciences Division
* National Institute of Technology,
* Gaithersburg, MD USA
*
*
* This software was developed at the National Institute of Standards and
* Technology (NIST) by employees of the Federal Government in the course
* of their official duties. Pursuant to title 17 Section 105 of the
* United States Code, this software is not subject to copyright protection
* and is in the public domain.  The Template Numerical Toolkit (TNT) is
* an experimental system.  NIST assumes no responsibility whatsoever for
* its use by other parties, and makes no guarantees, expressed or implied,
* about its quality, reliability, or any other characteristic.
*
* BETA VERSION INCOMPLETE AND SUBJECT TO CHANGE
* see http://math.nist.gov/tnt for latest updates.
*
*/



#ifndef CHOLESKY_H
#define CHOLESKY_H

#include <cmath>

// index method

namespace TNT
{


//
// Only upper part of A is used.  Cholesky factor is returned in
// lower part of L.  Returns 0 if successful, 1 otherwise.
//
template <class SPDMatrix, class SymmMatrix>
int Cholesky_upper_factorization(SPDMatrix &A, SymmMatrix &L)
{
    Subscript M = A.dim(1);
    Subscript N = A.dim(2);

    assert(M == N);                 // make sure A is square

    // readjust size of L, if necessary

    if (M != L.dim(1) || N != L.dim(2))
        L = SymmMatrix(N,N);

    Subscript i,j,k;


    typename SPDMatrix::element_type dot=0;


    for (j=1; j<=N; j++)                // form column j of L
    {
        dot= 0;

        for (i=1; i<j; i++)             // for k= 1 TO j-1
            dot = dot +  L(j,i)*L(j,i);

        L(j,j) = A(j,j) - dot;

        for (i=j+1; i<=N; i++)
        {
            dot = 0;
            for (k=1; k<j; k++)
                dot = dot +  L(i,k)*L(j,k);
            L(i,j) = A(j,i) - dot;
        }

        if (L(j,j) <= 0.0) return 1;

        L(j,j) = sqrt( L(j,j) );

        for (i=j+1; i<=N; i++)
            L(i,j) = L(i,j) / L(j,j);

    }

    return 0;
}




}  
// namespace TNT

#endif
// CHOLESKY_H
