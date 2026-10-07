/*
*	rmx Library
*	Copyright (C) 2008-2026 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*
*	VertexArrayObject
*		Support for OpenGL VAOs.
*/

#pragma once

#ifdef RMX_WITH_OPENGL_SUPPORT

// PS3 (PSGL + Cg): no VAOs, attributes are bound through the Cg vertex program, vertex data is kept in client memory
#if defined(PLATFORM_PS3) || defined(RMX_PLATFORM_PS3) || defined(__CELLOS_LV2__) || defined(__SNC__)
	#ifndef RMX_OPENGL_PS3_CG
		#define RMX_OPENGL_PS3_CG
	#endif
	#include <vector>
	void ps3DrawArrays(GLenum mode, GLint first, GLsizei count);	// Global function, see PS3GLCompat.h
#endif

namespace opengl
{
	class VertexArrayObject
	{
	public:
		enum class Format
		{
			UNDEFINED,
			P2,			// 2D position
			P2_C3,		// 2D position, RGB color
			P2_C4,		// 2D position, RGBA color
			P2_T2,		// 2D position, 2D texcoords
			P3_C3,		// 3D position, RGB color
			P3_N3_C3,	// 3D position, normal vector, RGB color
						// ...add more as needed
		};

	public:
		~VertexArrayObject();

		inline bool isValid() const  { return mVertexBufferObjectHandle != 0; }
		void setup(Format format);

		inline size_t getNumBufferedVertices() const  { return mNumBufferedVertices; }
		void updateVertexData(const float* vertexData, size_t numVertices);

		void bind();
		void unbind() const;

		void draw(GLenum mode);		// Shortcut for "bind()" + "glDrawArrays(mode, 0, mNumBufferedVertices)"

	private:
		void applyCurrentFormat();

	private:
		GLuint mVertexBufferObjectHandle = 0;	// We could actually use multiple VBOs (e.g. one for positions, one for texcoords), but one is sufficient
		GLuint mVertexArrayObjectHandle = 0;	// Vertex array object handle (only used if VAOs are actually supported, depending on the platform)
		Format mCurrentFormat = Format::UNDEFINED;

		size_t mNumBufferedVertices = 0;
		size_t mNumVertexAttributes = 0;
		size_t mFloatsPerVertex = 0;

	#ifdef RMX_OPENGL_PS3_CG
	private:
		friend void ::ps3DrawArrays(GLenum mode, GLint first, GLsizei count);
		static VertexArrayObject* sCurrent;						// The VAO that was bound last (defined in VertexArrayObject.cpp)
		std::vector<float> mClientData;							// Vertex data in client memory (mVertexBufferObjectHandle is only a "valid" marker on PS3)
	#endif
	};
}

#endif