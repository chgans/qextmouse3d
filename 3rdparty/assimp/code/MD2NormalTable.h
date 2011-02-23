/*
Open Asset Import Library (ASSIMP)
----------------------------------------------------------------------

Copyright (c) 2006-2010, ASSIMP Development Team
All rights reserved.

Redistribution and use of this software in source and binary forms,
with or without modification, are permitted provided that the
following conditions are met:

* Redistributions of source code must retain the above
  copyright notice, this list of conditions and the
  following disclaimer.

* Redistributions in binary form must reproduce the above
  copyright notice, this list of conditions and the
  following disclaimer in the documentation and/or other
  materials provided with the distribution.

* Neither the name of the ASSIMP team, nor the names of its
  contributors may be used to endorse or promote products
  derived from this software without specific prior
  written permission of the ASSIMP Development Team.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

----------------------------------------------------------------------
*/

/*
 * @file Slightly modified version of the anorms.h header file
 *  released by ID software with the Quake 2 source code.
 *
 * Table of normals used by MD2 models
 */

#ifndef AI_MDL_NORMALTABLE_H_INC
#define AI_MDL_NORMALTABLE_H_INC


float g_avNormals[162][3] = {
{ -0.525731f,  0.000000f,  0.850651f },
{ -0.442863f,  0.238856f,  0.864188f },
{ -0.295242f,  0.000000f,  0.955423f },
{ -0.309017f,  0.500000f,  0.809017f },
{ -0.162460f,  0.262866f,  0.951056f },
{  0.000000f,  0.000000f,  1.000000f },
{  0.000000f,  0.850651f,  0.525731f },
{ -0.147621f,  0.716567f,  0.681718f },
{  0.147621f,  0.716567f,  0.681718f },
{  0.000000f,  0.525731f,  0.850651f },
{  0.309017f,  0.500000f,  0.809017f },
{  0.525731f,  0.000000f,  0.850651f },
{  0.295242f,  0.000000f,  0.955423f },
{  0.442863f,  0.238856f,  0.864188f },
{  0.162460f,  0.262866f,  0.951056f },
{ -0.681718f,  0.147621f,  0.716567f },
{ -0.809017f,  0.309017f,  0.500000f },
{ -0.587785f,  0.425325f,  0.688191f },
{ -0.850651f,  0.525731f,  0.000000f },
{ -0.864188f,  0.442863f,  0.238856f },
{ -0.716567f,  0.681718f,  0.147621f },
{ -0.688191f,  0.587785f,  0.425325f },
{ -0.500000f,  0.809017f,  0.309017f },
{ -0.238856f,  0.864188f,  0.442863f },
{ -0.425325f,  0.688191f,  0.587785f },
{ -0.716567f,  0.681718f, -0.147621f },
{ -0.500000f,  0.809017f, -0.309017f },
{ -0.525731f,  0.850651f,  0.000000f },
{  0.000000f,  0.850651f, -0.525731f },
{ -0.238856f,  0.864188f, -0.442863f },
{  0.000000f,  0.955423f, -0.295242f },
{ -0.262866f,  0.951056f, -0.162460f },
{  0.000000f,  1.000000f,  0.000000f },
{  0.000000f,  0.955423f,  0.295242f },
{ -0.262866f,  0.951056f,  0.162460f },
{  0.238856f,  0.864188f,  0.442863f },
{  0.262866f,  0.951056f,  0.162460f },
{  0.500000f,  0.809017f,  0.309017f },
{  0.238856f,  0.864188f, -0.442863f },
{  0.262866f,  0.951056f, -0.162460f },
{  0.500000f,  0.809017f, -0.309017f },
{  0.850651f,  0.525731f,  0.000000f },
{  0.716567f,  0.681718f,  0.147621f },
{  0.716567f,  0.681718f, -0.147621f },
{  0.525731f,  0.850651f,  0.000000f },
{  0.425325f,  0.688191f,  0.587785f },
{  0.864188f,  0.442863f,  0.238856f },
{  0.688191f,  0.587785f,  0.425325f },
{  0.809017f,  0.309017f,  0.500000f },
{  0.681718f,  0.147621f,  0.716567f },
{  0.587785f,  0.425325f,  0.688191f },
{  0.955423f,  0.295242f,  0.000000f },
{  1.000000f,  0.000000f,  0.000000f },
{  0.951056f,  0.162460f,  0.262866f },
{  0.850651f, -0.525731f,  0.000000f },
{  0.955423f, -0.295242f,  0.000000f },
{  0.864188f, -0.442863f,  0.238856f },
{  0.951056f, -0.162460f,  0.262866f },
{  0.809017f, -0.309017f,  0.500000f },
{  0.681718f, -0.147621f,  0.716567f },
{  0.850651f,  0.000000f,  0.525731f },
{  0.864188f,  0.442863f, -0.238856f },
{  0.809017f,  0.309017f, -0.500000f },
{  0.951056f,  0.162460f, -0.262866f },
{  0.525731f,  0.000000f, -0.850651f },
{  0.681718f,  0.147621f, -0.716567f },
{  0.681718f, -0.147621f, -0.716567f },
{  0.850651f,  0.000000f, -0.525731f },
{  0.809017f, -0.309017f, -0.500000f },
{  0.864188f, -0.442863f, -0.238856f },
{  0.951056f, -0.162460f, -0.262866f },
{  0.147621f,  0.716567f, -0.681718f },
{  0.309017f,  0.500000f, -0.809017f },
{  0.425325f,  0.688191f, -0.587785f },
{  0.442863f,  0.238856f, -0.864188f },
{  0.587785f,  0.425325f, -0.688191f },
{  0.688191f,  0.587785f, -0.425325f },
{ -0.147621f,  0.716567f, -0.681718f },
{ -0.309017f,  0.500000f, -0.809017f },
{  0.000000f,  0.525731f, -0.850651f },
{ -0.525731f,  0.000000f, -0.850651f },
{ -0.442863f,  0.238856f, -0.864188f },
{ -0.295242f,  0.000000f, -0.955423f },
{ -0.162460f,  0.262866f, -0.951056f },
{  0.000000f,  0.000000f, -1.000000f },
{  0.295242f,  0.000000f, -0.955423f },
{  0.162460f,  0.262866f, -0.951056f },
{ -0.442863f, -0.238856f, -0.864188f },
{ -0.309017f, -0.500000f, -0.809017f },
{ -0.162460f, -0.262866f, -0.951056f },
{  0.000000f, -0.850651f, -0.525731f },
{ -0.147621f, -0.716567f, -0.681718f },
{  0.147621f, -0.716567f, -0.681718f },
{  0.000000f, -0.525731f, -0.850651f },
{  0.309017f, -0.500000f, -0.809017f },
{  0.442863f, -0.238856f, -0.864188f },
{  0.162460f, -0.262866f, -0.951056f },
{  0.238856f, -0.864188f, -0.442863f },
{  0.500000f, -0.809017f, -0.309017f },
{  0.425325f, -0.688191f, -0.587785f },
{  0.716567f, -0.681718f, -0.147621f },
{  0.688191f, -0.587785f, -0.425325f },
{  0.587785f, -0.425325f, -0.688191f },
{  0.000000f, -0.955423f, -0.295242f },
{  0.000000f, -1.000000f,  0.000000f },
{  0.262866f, -0.951056f, -0.162460f },
{  0.000000f, -0.850651f,  0.525731f },
{  0.000000f, -0.955423f,  0.295242f },
{  0.238856f, -0.864188f,  0.442863f },
{  0.262866f, -0.951056f,  0.162460f },
{  0.500000f, -0.809017f,  0.309017f },
{  0.716567f, -0.681718f,  0.147621f },
{  0.525731f, -0.850651f,  0.000000f },
{ -0.238856f, -0.864188f, -0.442863f },
{ -0.500000f, -0.809017f, -0.309017f },
{ -0.262866f, -0.951056f, -0.162460f },
{ -0.850651f, -0.525731f,  0.000000f },
{ -0.716567f, -0.681718f, -0.147621f },
{ -0.716567f, -0.681718f,  0.147621f },
{ -0.525731f, -0.850651f,  0.000000f },
{ -0.500000f, -0.809017f,  0.309017f },
{ -0.238856f, -0.864188f,  0.442863f },
{ -0.262866f, -0.951056f,  0.162460f },
{ -0.864188f, -0.442863f,  0.238856f },
{ -0.809017f, -0.309017f,  0.500000f },
{ -0.688191f, -0.587785f,  0.425325f },
{ -0.681718f, -0.147621f,  0.716567f },
{ -0.442863f, -0.238856f,  0.864188f },
{ -0.587785f, -0.425325f,  0.688191f },
{ -0.309017f, -0.500000f,  0.809017f },
{ -0.147621f, -0.716567f,  0.681718f },
{ -0.425325f, -0.688191f,  0.587785f },
{ -0.162460f, -0.262866f,  0.951056f },
{  0.442863f, -0.238856f,  0.864188f },
{  0.162460f, -0.262866f,  0.951056f },
{  0.309017f, -0.500000f,  0.809017f },
{  0.147621f, -0.716567f,  0.681718f },
{  0.000000f, -0.525731f,  0.850651f },
{  0.425325f, -0.688191f,  0.587785f },
{  0.587785f, -0.425325f,  0.688191f },
{  0.688191f, -0.587785f,  0.425325f },
{ -0.955423f,  0.295242f,  0.000000f },
{ -0.951056f,  0.162460f,  0.262866f },
{ -1.000000f,  0.000000f,  0.000000f },
{ -0.850651f,  0.000000f,  0.525731f },
{ -0.955423f, -0.295242f,  0.000000f },
{ -0.951056f, -0.162460f,  0.262866f },
{ -0.864188f,  0.442863f, -0.238856f },
{ -0.951056f,  0.162460f, -0.262866f },
{ -0.809017f,  0.309017f, -0.500000f },
{ -0.864188f, -0.442863f, -0.238856f },
{ -0.951056f, -0.162460f, -0.262866f },
{ -0.809017f, -0.309017f, -0.500000f },
{ -0.681718f,  0.147621f, -0.716567f },
{ -0.681718f, -0.147621f, -0.716567f },
{ -0.850651f,  0.000000f, -0.525731f },
{ -0.688191f,  0.587785f, -0.425325f },
{ -0.587785f,  0.425325f, -0.688191f },
{ -0.425325f,  0.688191f, -0.587785f },
{ -0.425325f, -0.688191f, -0.587785f },
{ -0.587785f, -0.425325f, -0.688191f },
{ -0.688191f, -0.587785f, -0.425325f }
};

#endif // !! include guard
