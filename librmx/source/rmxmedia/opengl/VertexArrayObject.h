/*
*	rmx Library
*	Copyright (C) 2008-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*
*	VertexArrayObject
*		Support for OpenGL VAOs.
*/

#pragma once

#ifdef RMX_WITH_OPENGL_SUPPORT

namespace opengl
{
	class VertexArrayObject
	{
	public:
		enum Format
		{
			Format_UNDEFINED,
			Format_P2,			// 2D position
			Format_P2_C3,		// 2D position, RGB color
			Format_P2_C4,		// 2D position, RGBA color
			Format_P2_T2		// 2D position, 2D texcoords
						// ...add more as needed
		};

	public:
		VertexArrayObject();
		~VertexArrayObject();

		void setup(Format format);

		inline size_t getNumBufferedVertices() const  { return mNumBufferedVertices; }
		void updateVertexData(const float* vertexData, size_t numVertices);

		void bind();
		void unbind();

		void draw(unsigned int mode);		// Shortcut for "bind()" + "glDrawArrays(mode, 0, mNumBufferedVertices)"

	private:
		unsigned int mHandle;						// Vertex array object handle
		unsigned int mVertexBufferObjectHandle;	// We could actually use multiple VBOs (e.g. one for positions, one for texcoords), but one is sufficient
		Format mCurrentFormat;

		size_t mNumBufferedVertices;
		size_t mNumVertexAttributes;
		size_t mFloatsPerVertex;
	};
}

#endif
