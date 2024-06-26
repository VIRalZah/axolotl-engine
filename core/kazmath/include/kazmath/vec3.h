/*
Copyright (c) 2008, Luke Benstead.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef VEC3_H_INCLUDED
#define VEC3_H_INCLUDED

#include "platform/PlatformMacros.h"
#include <assert.h>

#ifndef kmScalar
#define kmScalar float
#endif

struct kmMat4;

typedef struct kmVec3 {
    kmScalar x;
    kmScalar y;
    kmScalar z;
} kmVec3;

#ifdef __cplusplus
extern "C" {
#endif

AX_DLL kmVec3* kmVec3Fill(kmVec3* pOut, kmScalar x, kmScalar y, kmScalar z);
AX_DLL kmScalar kmVec3Length(const kmVec3* pIn); /** Returns the length of the vector */
AX_DLL kmScalar kmVec3LengthSq(const kmVec3* pIn); /** Returns the square of the length of the vector */
AX_DLL kmVec3* kmVec3Normalize(kmVec3* pOut, const kmVec3* pIn); /** Returns the vector passed in set to unit length */
AX_DLL kmVec3* kmVec3Cross(kmVec3* pOut, const kmVec3* pV1, const kmVec3* pV2); /** Returns a vector perpendicular to 2 other vectors */
AX_DLL kmScalar kmVec3Dot(const kmVec3* pV1, const kmVec3* pV2); /** Returns the cosine of the angle between 2 vectors */
AX_DLL kmVec3* kmVec3Add(kmVec3* pOut, const kmVec3* pV1, const kmVec3* pV2); /** Adds 2 vectors and returns the result */
AX_DLL kmVec3* kmVec3Subtract(kmVec3* pOut, const kmVec3* pV1, const kmVec3* pV2); /** Subtracts 2 vectors and returns the result */
AX_DLL kmVec3* kmVec3Transform(kmVec3* pOut, const kmVec3* pV1, const struct kmMat4* pM); /** Transforms a vector (assuming w=1) by a given matrix */
AX_DLL kmVec3* kmVec3TransformNormal(kmVec3* pOut, const kmVec3* pV, const struct kmMat4* pM);/**Transforms a 3D normal by a given matrix */
AX_DLL kmVec3* kmVec3TransformCoord(kmVec3* pOut, const kmVec3* pV, const struct kmMat4* pM); /**Transforms a 3D vector by a given matrix, projecting the result back into w = 1. */
AX_DLL kmVec3* kmVec3Scale(kmVec3* pOut, const kmVec3* pIn, const kmScalar s); /** Scales a vector to length s */
AX_DLL int kmVec3AreEqual(const kmVec3* p1, const kmVec3* p2);
AX_DLL kmVec3* kmVec3InverseTransform(kmVec3* pOut, const kmVec3* pV, const struct kmMat4* pM);
AX_DLL kmVec3* kmVec3InverseTransformNormal(kmVec3* pOut, const kmVec3* pVect, const struct kmMat4* pM);
AX_DLL kmVec3* kmVec3Assign(kmVec3* pOut, const kmVec3* pIn);
AX_DLL kmVec3* kmVec3Zero(kmVec3* pOut);

#ifdef __cplusplus
}
#endif
#endif /* VEC3_H_INCLUDED */
