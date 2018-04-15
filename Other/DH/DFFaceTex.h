#if !defined(_DF_FACETEX_H)
#define _DF_FACETEX_H


#include "../../DaggerTool.h"


/* Local type for matrix conversion parameters */
typedef struct {
	float  X[4],  Y[4],  Z[4];
	float  U[4],  V[4];
} df3duvparams_lt;


/* Used to convert XYZ point coordinates to DF UV coordinates */
typedef struct {
	float UA, UB, UC, UD;
	float VA, VB, VC, VD;
} df3duvmatrix_t;


// Class to wrap Dave Humnprey's UV code into its own namespace
class DFFaceTex
{
public:		// methods
	BOOL	ComputeDFFaceTextureUVMatrix( df3duvmatrix_t& Matrix, LPOBJVERTEX pFaceVerts );

private:	// methods
	BOOL	l_ComputeDFUVMatrixXY( df3duvmatrix_t& Matrix, const df3duvparams_lt& Params );
	BOOL	l_ComputeDFUVMatrixXZ( df3duvmatrix_t& Matrix, const df3duvparams_lt& Params );
	BOOL	l_ComputeDFUVMatrixYZ( df3duvmatrix_t& Matrix, const df3duvparams_lt& Params );
	BOOL	l_ComputeDFUVMatrixXYZ( df3duvmatrix_t& Matrix, const df3duvparams_lt& Params );
	BOOL	l_ComputeDFUVMatrixXYZ1( df3duvmatrix_t& Matrix, const df3duvparams_lt& Params );
};


#endif