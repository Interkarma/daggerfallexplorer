/*===========================================================================
 *
 * File:	DF_3DSTex.CPP
 * Author:	Dave Humphrey (uesp@m0use.net)
 * Created On:	Friday, March 22, 2002
 *
 * Implements 3DS specific texture and material related export functions 
 * for Daggerfall 3D objects.
 *
 *=========================================================================*/
/*
 * This version modified on 07/05/2002 by Gavin Clayton for Daggerfall Explorer.
 * For the original version of this file, please go to http://m0use.net/~uesp
 * and download DFTo3DS by Dave Humprey.
 *
*/


	/* Include Files */
#include <stdafx.h>
#include "DFFaceTex.h"


/*===========================================================================
 *
 * Local Function - boolean l_ComputeDFUVMatrixXY (Matrix, Params);
 *
 * Computes the UV conversion parameters from the given input based on
 * the formula:
 *			U = AX + BY + D
 *
 * Returns FALSE on any error.  For use on faces with 0 Z-coordinates.
 *
 *=========================================================================*/
 BOOL DFFaceTex::l_ComputeDFUVMatrixXY (df3duvmatrix_t& Matrix, const df3duvparams_lt& Params)
 {
  float Determinant;
  float Xi[3], Yi[3], Zi[3];

	/* Compute the determinant of the coefficient matrix */
  Determinant = Params.X[0]*Params.Y[1] + Params.Y[0]*Params.X[2] + 
		Params.X[1]*Params.Y[2] - Params.Y[1]*Params.X[2] - 
		Params.Y[0]*Params.X[1] - Params.X[0]*Params.Y[2];

	/* Check for a singular matrix indicating no valid solution */
  if (Determinant == 0) {
    return (FALSE);
   }

	/* Compute parameters of the the inverted XYZ matrix */
  Xi[0] = ( Params.Y[1] - Params.Y[2]) / Determinant;
  Xi[1] = (-Params.X[1] + Params.X[2]) / Determinant;
  Xi[2] = ( Params.X[1]*Params.Y[2] - Params.X[2]*Params.Y[1]) / Determinant;

  Yi[0] = (-Params.Y[0] + Params.Y[2]) / Determinant;
  Yi[1] = ( Params.X[0] - Params.X[2]) / Determinant;
  Yi[2] = (-Params.X[0]*Params.Y[2] + Params.X[2]*Params.Y[0]) / Determinant;

  Zi[0] = ( Params.Y[0] - Params.Y[1]) / Determinant;
  Zi[1] = (-Params.X[0] + Params.X[1]) / Determinant;
  Zi[2] = ( Params.X[0]*Params.Y[1] - Params.X[1]*Params.Y[0]) / Determinant;

	/* Compute the UV conversion parameters */
  Matrix.UA = (Params.U[0]*Xi[0] + Params.U[1]*Yi[0] + Params.U[2]*Zi[0]);
  Matrix.UB = (Params.U[0]*Xi[1] + Params.U[1]*Yi[1] + Params.U[2]*Zi[1]);
  Matrix.UC = (float) 0.0;
  Matrix.UD = (Params.U[0]*Xi[2] + Params.U[1]*Yi[2] + Params.U[2]*Zi[2]);;

  Matrix.VA = (Params.V[0]*Xi[0] + Params.V[1]*Yi[0] + Params.V[2]*Zi[0]);
  Matrix.VB = (Params.V[0]*Xi[1] + Params.V[1]*Yi[1] + Params.V[2]*Zi[1]);
  Matrix.VC = (float) 0.0;
  Matrix.VD = (Params.V[0]*Xi[2] + Params.V[1]*Yi[2] + Params.V[2]*Zi[2]);

  return (TRUE);
 }
/*===========================================================================
 *		End of Function l_ComputeDFUVMatrixXY()
 *=========================================================================*/


/*===========================================================================
 *
 * Local Function - boolean l_ComputeDFUVMatrixXZ (Matrix, Params);
 *
 * Computes the UV conversion parameters from the given input based on
 * the formula:
 *			U = AX + CZ + D
 *
 * Returns FALSE on any error.  For use on faces with 0 Y-coordinates.
 *
 *=========================================================================*/
BOOL DFFaceTex::l_ComputeDFUVMatrixXZ (df3duvmatrix_t& Matrix, const df3duvparams_lt& Params)
{
  float Determinant;
  float Xi[3], Yi[3], Zi[3];

	/* Compute the determinant of the coefficient matrix */
  Determinant = Params.X[0]*Params.Z[2] + Params.Z[1]*Params.X[2] +
		Params.Z[0]*Params.X[1] - Params.Z[0]*Params.X[2] - 
		Params.X[1]*Params.Z[2] - Params.X[0]*Params.Z[1];

	/* Check for a singular matrix indicating no valid solution */
  if (Determinant == 0) {
    return (FALSE);
   }

	/* Compute parameters of the the inverted XYZ matrix */
  Xi[0] = ( Params.Z[2] - Params.Z[1]) / Determinant;
  Xi[1] = (-Params.X[1]*Params.Z[2] + Params.X[2]*Params.Z[1]) / Determinant;
  Xi[2] = ( Params.X[1] - Params.X[2]) / Determinant;

  Yi[0] = (-Params.Z[2] + Params.Z[0]) / Determinant;
  Yi[1] = ( Params.X[0]*Params.Z[2] - Params.X[2]*Params.Z[0]) / Determinant;
  Yi[2] = (-Params.X[0] + Params.X[2]) / Determinant;

  Zi[0] = ( Params.Z[1] - Params.Z[0]) / Determinant;
  Zi[1] = (-Params.X[0]*Params.Z[1] + Params.X[1]*Params.Z[0]) / Determinant;
  Zi[2] = ( Params.X[0] - Params.X[1]) / Determinant;

	/* Compute the UV conversion parameters */
  Matrix.UA = (Params.U[0]*Xi[0] + Params.U[1]*Yi[0] + Params.U[2]*Zi[0]);
  Matrix.UB = (float) 0.0;
  Matrix.UC = (Params.U[0]*Xi[2] + Params.U[1]*Yi[2] + Params.U[2]*Zi[2]);
  Matrix.UD = (Params.U[0]*Xi[1] + Params.U[1]*Yi[1] + Params.U[2]*Zi[1]);

  Matrix.VA = (Params.V[0]*Xi[0] + Params.V[1]*Yi[0] + Params.V[2]*Zi[0]);
  Matrix.VB = (float) 0.0;
  Matrix.VC = (Params.V[0]*Xi[2] + Params.V[1]*Yi[2] + Params.V[2]*Zi[2]);
  Matrix.VD = (Params.V[0]*Xi[1] + Params.V[1]*Yi[1] + Params.V[2]*Zi[1]);

  return (TRUE);
 }
/*===========================================================================
 *		End of Function l_ComputeDFUVMatrixXZ()
 *=========================================================================*/


/*===========================================================================
 *
 * Local Function - boolean l_ComputeDFUVMatrixYZ (Matrix, Params);
 *
 * Computes the UV conversion parameters from the given input based on
 * the formula:
 *			U = BY + CZ + D
 *
 * Returns FALSE on any error.  For use on faces with 0 X-coordinates.
 *
 *=========================================================================*/
BOOL DFFaceTex::l_ComputeDFUVMatrixYZ (df3duvmatrix_t& Matrix, const df3duvparams_lt& Params)
{
  float Determinant;
  float Xi[3], Yi[3], Zi[3];

	/* Compute the determinant of the coefficient matrix */
  Determinant = Params.Y[1]*Params.Z[2] + Params.Y[0]*Params.Z[1] + 
		Params.Z[0]*Params.Y[2] - Params.Z[0]*Params.Y[1] - 
		Params.Y[0]*Params.Z[2] - Params.Z[1]*Params.Y[2];

	/* Check for a singular matrix indicating no valid solution */
  if (Determinant == 0) {
    return (FALSE);
   }

	/* Compute parameters of the the inverted XYZ matrix */
  Xi[0] = ( Params.Y[1]*Params.Z[2] - Params.Y[2]*Params.Z[1]) / Determinant;
  Xi[1] = (-Params.Z[2] + Params.Z[1]) / Determinant;
  Xi[2] = ( Params.Y[2] - Params.Y[1]) / Determinant;

  Yi[0] = (-Params.Y[0]*Params.Z[2] + Params.Y[2]*Params.Z[0]) / Determinant;
  Yi[1] = ( Params.Z[2] - Params.Z[0]) / Determinant;
  Yi[2] = (-Params.Y[2] + Params.Y[0]) / Determinant;

  Zi[0] = ( Params.Y[0]*Params.Z[1] - Params.Y[1]*Params.Z[0]) / Determinant;
  Zi[1] = (-Params.Z[1] + Params.Z[0]) / Determinant;
  Zi[2] = ( Params.Y[1] - Params.Y[0]) / Determinant;

	/* Compute the UV conversion parameters */
  Matrix.UA = (float) 0.0;
  Matrix.UB = (Params.U[0]*Xi[1] + Params.U[1]*Yi[1] + Params.U[2]*Zi[1]);
  Matrix.UC = (Params.U[0]*Xi[2] + Params.U[1]*Yi[2] + Params.U[2]*Zi[2]);
  Matrix.UD = (Params.U[0]*Xi[0] + Params.U[1]*Yi[0] + Params.U[2]*Zi[0]);

  Matrix.VA = (float) 0.0;
  Matrix.VB = (Params.V[0]*Xi[1] + Params.V[1]*Yi[1] + Params.V[2]*Zi[1]);
  Matrix.VC = (Params.V[0]*Xi[2] + Params.V[1]*Yi[2] + Params.V[2]*Zi[2]);
  Matrix.VD = (Params.V[0]*Xi[0] + Params.V[1]*Yi[0] + Params.V[2]*Zi[0]);

  return (TRUE);
 }
/*===========================================================================
 *		End of Function l_ComputeDFUVMatrixYZ()
 *=========================================================================*/


/*===========================================================================
 *
 * Local Function - boolean l_ComputeDFUVMatrixXYZ (Matrix, Params);
 *
 * Computes the UV conversion parameters from the given input based on
 * the formula:
 *			U = AX + BY + CZ
 *
 * Returns FALSE on any error.
 *
 *=========================================================================*/
BOOL DFFaceTex::l_ComputeDFUVMatrixXYZ (df3duvmatrix_t& Matrix, const df3duvparams_lt& Params)
{
  float Determinant;
  float Xi[3], Yi[3], Zi[3];

	/* Compute the determinant of the coefficient matrix */
  Determinant = Params.X[0]*Params.Y[1]*Params.Z[2] + 
		Params.Y[0]*Params.Z[1]*Params.X[2] + 
		Params.Z[0]*Params.X[1]*Params.Y[2] - 
		Params.Z[0]*Params.Y[1]*Params.X[2] - 
		Params.Y[0]*Params.X[1]*Params.Z[2] - 
		Params.X[0]*Params.Z[1]*Params.Y[2];

	/* Check for a singular matrix indicating no valid solution */
  if (Determinant == 0) {
    return (FALSE);
   }

	/* Compute values of the the inverted XYZ matrix */
  Xi[0] = ( Params.Y[1]*Params.Z[2] - Params.Y[2]*Params.Z[1]) / Determinant;
  Xi[1] = (-Params.X[1]*Params.Z[2] + Params.X[2]*Params.Z[1]) / Determinant;
  Xi[2] = ( Params.X[1]*Params.Y[2] - Params.X[2]*Params.Y[1]) / Determinant;

  Yi[0] = (-Params.Y[0]*Params.Z[2] + Params.Y[2]*Params.Z[0]) / Determinant;
  Yi[1] = ( Params.X[0]*Params.Z[2] - Params.X[2]*Params.Z[0]) / Determinant;
  Yi[2] = (-Params.X[0]*Params.Y[2] + Params.X[2]*Params.Y[0]) / Determinant;

  Zi[0] = ( Params.Y[0]*Params.Z[1] - Params.Y[1]*Params.Z[0]) / Determinant;
  Zi[1] = (-Params.X[0]*Params.Z[1] + Params.X[1]*Params.Z[0]) / Determinant;
  Zi[2] = ( Params.X[0]*Params.Y[1] - Params.X[1]*Params.Y[0]) / Determinant;

	/* Compute the UV conversion parameters */
  Matrix.UA = (Params.U[0]*Xi[0] + Params.U[1]*Yi[0] + Params.U[2]*Zi[0]);
  Matrix.UB = (Params.U[0]*Xi[1] + Params.U[1]*Yi[1] + Params.U[2]*Zi[1]);
  Matrix.UC = (Params.U[0]*Xi[2] + Params.U[1]*Yi[2] + Params.U[2]*Zi[2]);
  Matrix.UD = (float) 0.0;

  Matrix.VA = (Params.V[0]*Xi[0] + Params.V[1]*Yi[0] + Params.V[2]*Zi[0]);
  Matrix.VB = (Params.V[0]*Xi[1] + Params.V[1]*Yi[1] + Params.V[2]*Zi[1]);
  Matrix.VC = (Params.V[0]*Xi[2] + Params.V[1]*Yi[2] + Params.V[2]*Zi[2]);
  Matrix.VD = (float) 0.0;

  return (TRUE);
 }
/*===========================================================================
 *		End of Function l_ComputeDFUVMatrixXYZ()
 *=========================================================================*/


/*===========================================================================
 *
 * Local Function - boolean l_ComputeDFUVMatrixXYZ1 (Matrix, Params);
 *
 * Computes the UV conversion parameters from the given input based on
 * the formula:
 *			U = AX + BY + CZ + D
 *
 * Returns FALSE on any error.  Only valid for faces that have more than
 * 3 points.
 *
 *=========================================================================*/
BOOL DFFaceTex::l_ComputeDFUVMatrixXYZ1 (df3duvmatrix_t& Matrix, const df3duvparams_lt& Params)
{
  float Determinant;
  float Xi[4], Yi[4], Zi[4], Ai[4];

	/* Compute the determinant of the coefficient matrix */
  Determinant =  Params.X[0]*Params.Y[1]*Params.Z[2]
		-Params.X[0]*Params.Y[1]*Params.Z[3]
		+Params.X[0]*Params.Z[1]*Params.Y[3]
		-Params.X[0]*Params.Z[1]*Params.Y[2]
		+Params.X[0]*Params.Y[2]*Params.Z[3]
		-Params.X[0]*Params.Z[2]*Params.Y[3]
		-Params.Y[0]*Params.Z[1]*Params.X[3]
		+Params.Y[0]*Params.Z[1]*Params.X[2]
		-Params.Y[0]*Params.X[2]*Params.Z[3]
		+Params.Y[0]*Params.Z[2]*Params.X[3]
		-Params.Y[0]*Params.X[1]*Params.Z[2]
		+Params.Y[0]*Params.X[1]*Params.Z[3]
		+Params.Z[0]*Params.X[2]*Params.Y[3]
		-Params.Z[0]*Params.Y[2]*Params.X[3]
		+Params.Z[0]*Params.X[1]*Params.Y[2]
		-Params.Z[0]*Params.X[1]*Params.Y[3]
		+Params.Z[0]*Params.Y[1]*Params.X[3]
		-Params.Z[0]*Params.Y[1]*Params.X[2]
		-Params.X[1]*Params.Y[2]*Params.Z[3]
		+Params.X[1]*Params.Z[2]*Params.Y[3]
		-Params.Y[1]*Params.Z[2]*Params.X[3]
		+Params.Y[1]*Params.X[2]*Params.Z[3]
		-Params.Z[1]*Params.X[2]*Params.Y[3]
		+Params.Z[1]*Params.Y[2]*Params.X[3];

	/* Check for a singular matrix indicating no valid solution */
  if (Determinant == 0) {
    return (FALSE);
   }

	/* Compute values of the the inverted XYZ matrix */
  Xi[0] =  Params.Y[1]*Params.Z[2] + Params.Z[1]*Params.Y[3] + Params.Y[2]*Params.Z[3] -
	   Params.Y[3]*Params.Z[2] - Params.Y[2]*Params.Z[1] - Params.Y[1]*Params.Z[3];
  Xi[1]	= -Params.X[1]*Params.Z[2] - Params.Z[1]*Params.X[3] - Params.X[2]*Params.Z[3] + 
	   Params.X[3]*Params.Z[2] + Params.X[2]*Params.Z[1] + Params.X[1]*Params.Z[3];
  Xi[2] =  Params.X[1]*Params.Y[2] + Params.Y[1]*Params.X[3] + Params.X[2]*Params.Y[3] - 
	   Params.X[3]*Params.Y[2] - Params.X[2]*Params.Y[1] - Params.X[1]*Params.Y[3];
  Xi[3] = -Params.X[1]*Params.Y[2]*Params.Z[3] - Params.Y[1]*Params.Z[2]*Params.X[3] - Params.Z[1]*Params.X[2]*Params.Y[3] +
	   Params.X[3]*Params.Y[2]*Params.Z[1] + Params.X[2]*Params.Y[1]*Params.Z[3] + Params.X[1]*Params.Y[3]*Params.Z[2];
  Yi[0] = -Params.Y[0]*Params.Z[2] - Params.Z[0]*Params.Y[3] - Params.Y[2]*Params.Z[3] + 
	   Params.Y[3]*Params.Z[2] + Params.Y[2]*Params.Z[0] + Params.Y[0]*Params.Z[3];
  Yi[1] =  Params.X[0]*Params.Z[2] + Params.Z[0]*Params.X[3] + Params.X[2]*Params.Z[3] -
	   Params.X[3]*Params.Z[2] - Params.X[2]*Params.Z[0] - Params.X[0]*Params.Z[3];
  Yi[2] = -Params.X[0]*Params.Y[2] - Params.Y[0]*Params.X[3] - Params.X[2]*Params.Y[3] + 
	   Params.X[3]*Params.Y[2] + Params.X[2]*Params.Y[0] + Params.X[0]*Params.Y[3];
  Yi[3] =  Params.X[0]*Params.Y[2]*Params.Z[3] + Params.Y[0]*Params.Z[2]*Params.X[3] + Params.Z[0]*Params.X[2]*Params.Y[3] - 
	   Params.X[3]*Params.Y[2]*Params.Z[0] - Params.X[2]*Params.Y[0]*Params.Z[3] - Params.X[0]*Params.Y[3]*Params.Z[2];
  Zi[0] =  Params.Y[0]*Params.Z[1] + Params.Z[0]*Params.Y[3] + Params.Y[1]*Params.Z[3]
	  -Params.Y[3]*Params.Z[1] - Params.Y[0]*Params.Z[0] - Params.Y[0]*Params.Z[3];
  Zi[1] = -Params.X[0]*Params.Z[1] - Params.Z[0]*Params.Y[3] - Params.X[1]*Params.Z[3] +
	   Params.X[3]*Params.Z[1] + Params.X[1]*Params.Z[0] + Params.X[0]*Params.Z[3];
  Zi[2] =  Params.X[0]*Params.Y[1] + Params.Y[0]*Params.X[3] + Params.X[1]*Params.Y[3] -
	   Params.X[3]*Params.Y[1] - Params.X[1]*Params.Y[0] - Params.X[0]*Params.Y[3];
  Zi[3] = -Params.X[0]*Params.Y[1]*Params.Z[3] - Params.Y[0]*Params.Z[1]*Params.X[3] - Params.Z[0]*Params.X[1]*Params.Y[3] +
	   Params.X[3]*Params.Y[1]*Params.Z[0] + Params.Z[1]*Params.Y[0]*Params.Z[3] + Params.X[0]*Params.Y[3]*Params.Z[1];
  Ai[0] = -Params.Y[0]*Params.Z[1] - Params.Z[0]*Params.Y[2] - Params.Y[1]*Params.Z[2] +
	   Params.Y[2]*Params.Z[1] + Params.Y[1]*Params.Z[0] + Params.Y[0]*Params.Z[2];
  Ai[1] =  Params.X[0]*Params.Z[1] + Params.Z[0]*Params.X[2] + Params.X[1]*Params.Z[2] -
	   Params.X[1]*Params.Z[1] - Params.X[1]*Params.Z[0] - Params.X[0]*Params.Z[2];
  Ai[2] = -Params.X[0]*Params.Y[1] - Params.Y[0]*Params.X[2] - Params.X[1]*Params.Y[2] +
	   Params.X[2]*Params.Y[1] + Params.X[1]*Params.Y[0] + Params.X[0]*Params.Y[2];
  Ai[3] =  Params.X[0]*Params.Y[1]*Params.Z[2] + Params.Y[0]*Params.Z[1]*Params.X[2] + Params.Z[0]*Params.X[1]*Params.Y[2] -
	   Params.X[2]*Params.Y[1]*Params.Z[0] - Params.X[1]*Params.Y[0]*Params.Z[2] - Params.X[0]*Params.Y[2]*Params.Z[1];

	/* Compute the UV conversion parameters */
  Matrix.UA = (Params.U[0]*Xi[0] + Params.U[1]*Yi[0] + Params.U[2]*Zi[0] + Params.U[3]*Ai[0]) / Determinant;
  Matrix.UB = (Params.U[0]*Xi[1] + Params.U[1]*Yi[1] + Params.U[2]*Zi[1] + Params.U[3]*Ai[1]) / Determinant;
  Matrix.UC = (Params.U[0]*Xi[2] + Params.U[1]*Yi[2] + Params.U[2]*Zi[2] + Params.U[3]*Ai[2]) / Determinant;
  Matrix.UD = (Params.U[0]*Xi[3] + Params.U[1]*Yi[3] + Params.U[2]*Zi[3] + Params.U[3]*Ai[3]) / Determinant;

  Matrix.VA = (Params.V[0]*Xi[0] + Params.V[1]*Yi[0] + Params.V[2]*Zi[0] + Params.V[3]*Ai[0]) / Determinant;
  Matrix.VB = (Params.V[0]*Xi[1] + Params.V[1]*Yi[1] + Params.V[2]*Zi[1] + Params.V[3]*Ai[1]) / Determinant;
  Matrix.VC = (Params.V[0]*Xi[2] + Params.V[1]*Yi[2] + Params.V[2]*Zi[2] + Params.V[3]*Ai[2]) / Determinant;
  Matrix.VD = (Params.V[0]*Xi[3] + Params.V[1]*Yi[3] + Params.V[2]*Zi[3] + Params.V[3]*Ai[3]) / Determinant;

  return (TRUE);
 }
/*===========================================================================
 *		End of Function l_ComputeDFUVMatrixXYZ1()
 *=========================================================================*/



/*===========================================================================
 *
 * Function - boolean ComputeDFFaceTextureUVMatrix (Matrix, Face, Object);
 *
 * Calculates the transformation parameters to convert the face XYZ coordinates
 * to texture UV coordinates for the given face, storing in the given matrix.
 * Returns FALSE on any error.  The function computes the A, B...F parameters
 * for the equations:
 *			U = AX + BY + CZ
 *			V = DX + EY + FZ
 *
 *=========================================================================*/
 BOOL DFFaceTex::ComputeDFFaceTextureUVMatrix( df3duvmatrix_t& Matrix, LPOBJVERTEX pFaceVerts )
{
  df3duvparams_lt Params;
  BOOL	  Result;

	/* Initialize the conversion matrix */
  Matrix.UA = (float) 1.0;
  Matrix.UB = (float) 0.0;
  Matrix.UC = (float) 0.0;
  Matrix.UD = (float) 0.0;
  Matrix.VA = (float) 0.0;
  Matrix.VB = (float) 1.0;
  Matrix.VC = (float) 0.0;
  Matrix.UD = (float) 0.0;

	/* Store the first 3 points of texture coordinates */
  Params.U[0] = pFaceVerts[0].tu;
  Params.U[1] = pFaceVerts[1].tu;
  Params.U[2] = pFaceVerts[2].tu;
  Params.V[0] = pFaceVerts[0].tv;
  Params.V[1] = pFaceVerts[1].tv;
  Params.V[2] = pFaceVerts[2].tv;

	/* Get and store the 1st point coordinates in face */
  Params.X[0] = pFaceVerts[0].pos.x;
  Params.Y[0] = pFaceVerts[0].pos.y;
  Params.Z[0] = pFaceVerts[0].pos.z;

	/* Get and store the 2nd point coordinates in face */
  Params.X[1] = pFaceVerts[1].pos.x;
  Params.Y[1] = pFaceVerts[1].pos.y;
  Params.Z[1] = pFaceVerts[1].pos.z;

	/* Get and store the 3rd point coordinates in face */
  Params.X[2] = pFaceVerts[2].pos.x;
  Params.Y[2] = pFaceVerts[2].pos.y;
  Params.Z[2] = pFaceVerts[2].pos.z;

	/* get and store the 4th store coordinates, if any */
  // NOTE: CDaggerTool::SetFaceUV has already accounted for presence/absence of 4th point
  Params.X[3] = pFaceVerts[3].pos.x;
  Params.Y[3] = pFaceVerts[3].pos.y;
  Params.Z[3] = pFaceVerts[3].pos.z;
    
	/* Compute the solution using an XZ linear equation */
  if (Params.Y[0] == Params.Y[1] && Params.Y[1] == Params.Y[2]) {
    Result = l_ComputeDFUVMatrixXZ(Matrix, Params); 
   }
	/* Compute the solution using an XY linear equation */
  else if (Params.Z[0] == Params.Z[1] && Params.Z[1] == Params.Z[2]) {
    Result = l_ComputeDFUVMatrixXY(Matrix, Params); 
   }
	/* Compute the solution using an YZ linear equation */
  else if (Params.X[0] == Params.X[1] && Params.X[1] == Params.X[2]) {
    Result = l_ComputeDFUVMatrixYZ(Matrix, Params); 
   }
	/* Compute the solution using an XYZ linear equation */
  else {
    Result = l_ComputeDFUVMatrixXYZ(Matrix, Params);
   }

	/* Attempt to use a 4x4 matrix solution if the previous ones failed */
  if (!Result) {
    Result = l_ComputeDFUVMatrixXYZ1(Matrix, Params);
   }

  return (Result);
 }
/*===========================================================================
 *		End of Function ComputeDFFaceTextureUVMatrix()
 *=========================================================================*/



