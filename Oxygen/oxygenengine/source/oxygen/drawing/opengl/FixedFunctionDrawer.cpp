/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "oxygen/pch.h"

#ifdef RMX_WITH_OPENGL_SUPPORT

#include <PSGL/psgl.h>
#include <PSGL/psglu.h>
#define glOrthoPlatform glOrthof

#include "oxygen/drawing/opengl/FixedFunctionDrawer.h"
#include "oxygen/drawing/opengl/OpenGLDrawerTexture.h"
#include "oxygen/drawing/opengl/OpenGLSpriteTextureManager.h"
#include "oxygen/drawing/DrawCollection.h"
#include "oxygen/drawing/DrawCommand.h"
#include "oxygen/application/EngineMain.h"
#include "oxygen/helper/Logging.h"
#include "oxygen/resources/SpriteCollection.h"
#include "oxygen/rendering/sprite/ComponentSprite.h"
#include "oxygen/application/Application.h"
#include "oxygen/application/gameview/GameView.h"
#include "rmxmedia/opengl/OpenGLFontOutput.h"


namespace fixedfunctiondrawer
{
	struct Internal
	{
	public:
		Internal()
		{
#if defined(RMX_USE_GLEW)
			// GLEW initialization
			RMX_LOG_INFO("GLEW initialization...");
			const GLenum result = glewInit();
			if (result != GLEW_OK)
			{
				RMX_ERROR("Error in OpenGL initialization (glewInit):\n" << glewGetErrorString(result), );
				return;
			}
#endif

			// Setup OpenGL defaults
			RMX_LOG_INFO("Setting OpenGL defaults...");
			glDisable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_ALPHA_TEST);
			glDisable(GL_STENCIL_TEST);
			glDisable(GL_SCISSOR_TEST);
			glDisable(GL_LIGHTING);
			glDisable(GL_FOG);
			glDisable(GL_DITHER);

			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

			setBlendMode(BlendMode::OPAQUE);

			mSetupSuccessful = true;
		}

		~Internal()
		{
		}

		OpenGLDrawerTexture* createTexture(DrawerTexture& outTexture)
		{
			OpenGLDrawerTexture* texture = new OpenGLDrawerTexture(outTexture);
			return texture;
		}

		void setBlendMode(BlendMode blendMode)
		{
			mCurrentBlendMode = blendMode;
			switch (blendMode)
			{
			case BlendMode::OPAQUE:
				glDisable(GL_BLEND);
				break;
			case BlendMode::ALPHA:
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				break;
			case BlendMode::ADDITIVE:
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				break;
			}
		}

		void applySamplingMode(GLuint textureHandle)
		{
			glBindTexture(GL_TEXTURE_2D, textureHandle);
			switch (mCurrentSamplingMode)
			{
			case SamplingMode::POINT:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				break;
			case SamplingMode::BILINEAR:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				break;
			}
		}

		void applyWrapMode(GLuint textureHandle)
		{
			glBindTexture(GL_TEXTURE_2D, textureHandle);
			switch (mCurrentWrapMode)
			{
			case TextureWrapMode::CLAMP:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				break;
			case TextureWrapMode::REPEAT:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				break;
			}
		}

		GLuint setupTexture(DrawerTexture& drawerTexture)
		{
			OpenGLDrawerTexture* openGLDrawerTexture = drawerTexture.getImplementation<OpenGLDrawerTexture>();
			RMX_CHECK(nullptr != openGLDrawerTexture, "Invalid OpenGL texture implementation", return 0);
			const GLuint textureHandle = openGLDrawerTexture->getTextureHandle();

			if (openGLDrawerTexture->mSamplingMode != mCurrentSamplingMode || openGLDrawerTexture->mWrapMode != mCurrentWrapMode)
			{
				applySamplingMode(textureHandle);
				applyWrapMode(textureHandle);
				openGLDrawerTexture->mSamplingMode = mCurrentSamplingMode;
				openGLDrawerTexture->mWrapMode = mCurrentWrapMode;
			}
			return textureHandle;
		}

		OpenGLFontOutput& getOpenGLFontOutput(Font& font)
		{
			std::shared_ptr<OpenGLFontOutput>* fontOutputPtr = mapFind(mFontOutputMap, &font);
			if (nullptr != fontOutputPtr && *fontOutputPtr)
				return **fontOutputPtr;

			std::shared_ptr<OpenGLFontOutput> fontOutput(new OpenGLFontOutput(font));
			mFontOutputMap.insert(std::make_pair(&font, fontOutput));
			return *fontOutput;
		}

		void drawRect(const Recti& targetRect, GLuint textureHandle, const Color& color, Vec2f uv0 = Vec2f(0.0f, 0.0f), Vec2f uv1 = Vec2f(1.0f, 1.0f))
		{
			if (textureHandle != 0)
			{
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, textureHandle);
			}
			else
			{
				glDisable(GL_TEXTURE_2D);
			}

			glColor4f(color.r, color.g, color.b, color.a);

			const float vertices[] = {
				(float)targetRect.x, (float)targetRect.y,
				(float)targetRect.x + targetRect.width, (float)targetRect.y,
				(float)targetRect.x, (float)targetRect.y + targetRect.height,
				(float)targetRect.x + targetRect.width, (float)targetRect.y + targetRect.height
			};
			const float texcoords[] = {
				uv0.x, uv0.y,
				uv1.x, uv0.y,
				uv0.x, uv1.y,
				uv1.x, uv1.y
			};

			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(2, GL_FLOAT, 0, vertices);

			if (textureHandle != 0)
			{
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(2, GL_FLOAT, 0, texcoords);
			}

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

			glDisableClientState(GL_VERTEX_ARRAY);
			if (textureHandle != 0)
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}

		void applyScissor(const Recti& scissorRect)
		{
#if defined(PLATFORM_PS3) || defined(RMX_PLATFORM_PS3) || defined(__CELLOS_LV2__) || defined(__SNC__)
			const Vec2i screenSize = (FTX::Video.valid() && FTX::Video->isActive()) ? FTX::Video->getScreenSize() : Vec2i(1280, 720);
			const Recti displayRect = (mUpscaledRect.width > 0 && mUpscaledRect.height > 0) ? mUpscaledRect : Recti(0, 0, screenSize.x, screenSize.y);

			const int targetW = (mCurrentViewport.width > 0) ? mCurrentViewport.width : screenSize.x;
			const int targetH = (mCurrentViewport.height > 0) ? mCurrentViewport.height : screenSize.y;

			const float scaleX = (float)displayRect.width / (float)targetW;
			const float scaleY = (float)displayRect.height / (float)targetH;

			int glX = displayRect.x + roundToInt((float)scissorRect.x * scaleX);
			int glW = roundToInt((float)scissorRect.width * scaleX);
			int glH = roundToInt((float)scissorRect.height * scaleY);
			int glY = screenSize.y - (displayRect.y + roundToInt((float)(scissorRect.y + scissorRect.height) * scaleY));

			glX = std::max(glX, 0);
			glY = std::max(glY, 0);
			glW = std::max(glW, 0);
			glH = std::max(glH, 0);

			glScissor(glX, glY, glW, glH);
#else
			const int targetH = (mCurrentViewport.height > 0) ? mCurrentViewport.height : ((FTX::Video.valid() && FTX::Video->isActive()) ? FTX::Video->getScreenHeight() : 720);
			const int glX = scissorRect.x;
			const int glY = targetH - (scissorRect.y + scissorRect.height);
			const int glW = std::max(scissorRect.width, 0);
			const int glH = std::max(scissorRect.height, 0);
			glScissor(glX, std::max(glY, 0), glW, glH);
#endif
		}

		void printText(Font& font, const StringReader& text, const Recti& rect, const DrawerPrintOptions& printOptions)
		{
			OpenGLFontOutput& fontOutput = getOpenGLFontOutput(font);
			const Vec2i pos = font.alignText(rect, text, printOptions.mAlignment);

			static std::vector<Font::TypeInfo> typeInfos;
			typeInfos.clear();
			font.getTypeInfos(typeInfos, pos, text, printOptions.mSpacing);
			if (typeInfos.empty())
				return;

			static OpenGLFontOutput::VertexGroups vertexGroups;
			fontOutput.buildVertexGroups(vertexGroups, typeInfos);

			setBlendMode(BlendMode::ALPHA);
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GREATER, 0.05f);

			glEnable(GL_TEXTURE_2D);
			const Color& tintColor = printOptions.mTintColor;
			glColor4f(tintColor.r, tintColor.g, tintColor.b, tintColor.a);

			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

			for (const OpenGLFontOutput::VertexGroup& vertexGroup : vertexGroups.mVertexGroups)
			{
				GLuint textureHandle = vertexGroup.mTexture->getHandle();
				applySamplingMode(textureHandle);
				applyWrapMode(textureHandle);

				static std::vector<float> vertexData;
				static std::vector<float> uvData;
				vertexData.resize(vertexGroup.mNumVertices * 2);
				uvData.resize(vertexGroup.mNumVertices * 2);

				for (size_t i = 0; i < vertexGroup.mNumVertices; ++i)
				{
					const OpenGLFontOutput::Vertex& src = vertexGroups.mVertices[vertexGroup.mStartIndex + i];
					vertexData[i * 2 + 0] = src.mPosition.x;
					vertexData[i * 2 + 1] = src.mPosition.y;
					uvData[i * 2 + 0] = src.mTexcoords.x;
					uvData[i * 2 + 1] = src.mTexcoords.y;
				}

				glVertexPointer(2, GL_FLOAT, 0, &vertexData[0]);
				glTexCoordPointer(2, GL_FLOAT, 0, &uvData[0]);
				glDrawArrays(GL_TRIANGLES, 0, vertexGroup.mNumVertices);
			}

			glDisable(GL_ALPHA_TEST);

			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}

	public:
		bool mSetupSuccessful = false;
		SDL_Window* mOutputWindow = nullptr;
		OpenGLSpriteTextureManager mSpriteTextureManager;

		BlendMode mCurrentBlendMode = BlendMode::OPAQUE;
		SamplingMode mCurrentSamplingMode = SamplingMode::POINT;
		TextureWrapMode mCurrentWrapMode = TextureWrapMode::CLAMP;

		Recti mCurrentViewport;
		Recti mUpscaledRect;
		std::vector<Recti> mScissorStack;

	private:
		std::unordered_map<Font*, std::shared_ptr<OpenGLFontOutput>> mFontOutputMap;
	};
}


FixedFunctionDrawer::FixedFunctionDrawer() :
mInternal(*new fixedfunctiondrawer::Internal())
{
}

FixedFunctionDrawer::~FixedFunctionDrawer()
{
	delete &mInternal;
}

bool FixedFunctionDrawer::wasSetupSuccessful()
{
	return mInternal.mSetupSuccessful;
}

void FixedFunctionDrawer::createTexture(DrawerTexture& outTexture)
{
	outTexture.setImplementation(mInternal.createTexture(outTexture));
}

void FixedFunctionDrawer::refreshTexture(DrawerTexture& texture)
{
	createTexture(texture);
}

void FixedFunctionDrawer::setupRenderWindow(SDL_Window* window)
{
	mInternal.mOutputWindow = window;
}

void FixedFunctionDrawer::performRendering(const DrawCollection& drawCollection)
{
	for (DrawCommand* drawCommand : drawCollection.getDrawCommands())
	{
		const DrawCommand::Type type = drawCommand->getType();

		switch (type)
		{
		case DrawCommand::Type::SET_WINDOW_RENDER_TARGET:
		{
															SetWindowRenderTargetDrawCommand& dc = drawCommand->as<SetWindowRenderTargetDrawCommand>();
															glBindFramebuffer(GL_FRAMEBUFFER, 0);
															glViewport(dc.mViewport.x, dc.mViewport.y, dc.mViewport.width, dc.mViewport.height);
															mInternal.mCurrentViewport = dc.mViewport;
															const Vec2i screenSize = (FTX::Video.valid() && FTX::Video->isActive()) ? FTX::Video->getScreenSize() : Vec2i(1280, 720);
															mInternal.mUpscaledRect = Recti(0, 0, screenSize.x, screenSize.y);

															glMatrixMode(GL_PROJECTION);
															glLoadIdentity();
															glOrthoPlatform(0, (float)dc.mViewport.width, (float)dc.mViewport.height, 0, -1, 1);
															glMatrixMode(GL_MODELVIEW);
															glLoadIdentity();
															break;
		}

		case DrawCommand::Type::SET_RENDER_TARGET:
		{
													 SetRenderTargetDrawCommand& dc = drawCommand->as<SetRenderTargetDrawCommand>();
													 glBindFramebuffer(GL_FRAMEBUFFER, 0);
													 const Vec2i screenSize = FTX::Video->getScreenSize();

													 // Obtain active upscaled viewport rect from Application's GameView if available
													 Recti targetDisplayRect = Application::instance().getGameView().getGameViewport();
													 if (targetDisplayRect.width <= 0 || targetDisplayRect.height <= 0)
													 {
														 targetDisplayRect = Recti(0, 0, screenSize.x, screenSize.y);
													 }

													 mInternal.mUpscaledRect = targetDisplayRect;
													 // OpenGL glViewport y-axis starts at bottom-left corner
													 glViewport(targetDisplayRect.x, screenSize.y - (targetDisplayRect.y + targetDisplayRect.height), targetDisplayRect.width, targetDisplayRect.height);
													 mInternal.mCurrentViewport = dc.mViewport;

													 glMatrixMode(GL_PROJECTION);
													 glLoadIdentity();
													 glOrthoPlatform(0, (float)dc.mViewport.width, (float)dc.mViewport.height, 0, -1, 1);
													 glMatrixMode(GL_MODELVIEW);
													 glLoadIdentity();
													 break;
		}

		case DrawCommand::Type::RECT:
		{
										RectDrawCommand& dc = drawCommand->as<RectDrawCommand>();
										GLuint textureHandle = 0;
										if (nullptr != dc.mTexture)
											textureHandle = mInternal.setupTexture(*dc.mTexture);

										mInternal.drawRect(dc.mRect, textureHandle, dc.mColor, dc.mUV0, dc.mUV1);
										break;
		}

		case DrawCommand::Type::UPSCALED_RECT:
		{
												 // On PS3, rendering is performed directly on the main window backbuffer during SET_RENDER_TARGET
												 break;
		}

		case DrawCommand::Type::SPRITE:
		{
										  SpriteDrawCommand& sc = drawCommand->as<SpriteDrawCommand>();
										  const SpriteCollection::Item* item = SpriteCollection::instance().getSprite(sc.mSpriteKey);
										  if (nullptr == item || !item->mUsesComponentSprite)
											  break;

										  OpenGLTexture* texture = mInternal.mSpriteTextureManager.getComponentSpriteTexture(*item);
										  if (nullptr == texture)
											  break;

										  ComponentSprite& sprite = *static_cast<ComponentSprite*>(item->mSprite);
										  Vec2i offset = sprite.mOffset;
										  Vec2i size = sprite.getBitmap().getSize();
										  if (sc.mScale.x != 1.0f || sc.mScale.y != 1.0f)
										  {
											  offset.x = roundToInt((float)offset.x * sc.mScale.x);
											  offset.y = roundToInt((float)offset.y * sc.mScale.y);
											  size.x = roundToInt((float)size.x * sc.mScale.x);
											  size.y = roundToInt((float)size.y * sc.mScale.y);
										  }
										  const Recti targetRect(sc.mPosition + offset, size);

										  mInternal.drawRect(targetRect, texture->getHandle(), sc.mTintColor);
										  break;
		}

		case DrawCommand::Type::SPRITE_RECT:
		{
											   SpriteRectDrawCommand& sc = drawCommand->as<SpriteRectDrawCommand>();
											   const SpriteCollection::Item* item = SpriteCollection::instance().getSprite(sc.mSpriteKey);
											   if (nullptr == item || !item->mUsesComponentSprite)
												   break;

											   OpenGLTexture* texture = mInternal.mSpriteTextureManager.getComponentSpriteTexture(*item);
											   if (nullptr == texture)
												   break;

											   mInternal.drawRect(sc.mRect, texture->getHandle(), sc.mTintColor);
											   break;
		}

		case DrawCommand::Type::MESH:
		{
										MeshDrawCommand& dc = drawCommand->as<MeshDrawCommand>();
										if (dc.mTriangles.empty() || nullptr == dc.mTexture)
											break;

										GLuint textureHandle = mInternal.setupTexture(*dc.mTexture);
										glEnable(GL_TEXTURE_2D);
										glBindTexture(GL_TEXTURE_2D, textureHandle);
										glColor4f(1, 1, 1, 1);

										static std::vector<float> vertexData;
										static std::vector<float> uvData;
										vertexData.resize(dc.mTriangles.size() * 2);
										uvData.resize(dc.mTriangles.size() * 2);

										for (size_t i = 0; i < dc.mTriangles.size(); ++i)
										{
											vertexData[i * 2 + 0] = dc.mTriangles[i].mPosition.x;
											vertexData[i * 2 + 1] = dc.mTriangles[i].mPosition.y;
											uvData[i * 2 + 0] = dc.mTriangles[i].mTexcoords.x;
											uvData[i * 2 + 1] = dc.mTriangles[i].mTexcoords.y;
										}

										glEnableClientState(GL_VERTEX_ARRAY);
										glEnableClientState(GL_TEXTURE_COORD_ARRAY);
										glVertexPointer(2, GL_FLOAT, 0, &vertexData[0]);
										glTexCoordPointer(2, GL_FLOAT, 0, &uvData[0]);
										glDrawArrays(GL_TRIANGLES, 0, dc.mTriangles.size());
										glDisableClientState(GL_VERTEX_ARRAY);
										glDisableClientState(GL_TEXTURE_COORD_ARRAY);
										break;
		}

		case DrawCommand::Type::MESH_VERTEX_COLOR:
		{
													 MeshVertexColorDrawCommand& dc = drawCommand->as<MeshVertexColorDrawCommand>();
													 if (dc.mTriangles.empty())
														 break;

													 glDisable(GL_TEXTURE_2D);

													 static std::vector<float> vertexData;
													 static std::vector<float> colorData;
													 vertexData.resize(dc.mTriangles.size() * 2);
													 colorData.resize(dc.mTriangles.size() * 4);

													 for (size_t i = 0; i < dc.mTriangles.size(); ++i)
													 {
														 vertexData[i * 2 + 0] = dc.mTriangles[i].mPosition.x;
														 vertexData[i * 2 + 1] = dc.mTriangles[i].mPosition.y;
														 colorData[i * 4 + 0] = dc.mTriangles[i].mColor.r;
														 colorData[i * 4 + 1] = dc.mTriangles[i].mColor.g;
														 colorData[i * 4 + 2] = dc.mTriangles[i].mColor.b;
														 colorData[i * 4 + 3] = dc.mTriangles[i].mColor.a;
													 }

													 glEnableClientState(GL_VERTEX_ARRAY);
													 glEnableClientState(GL_COLOR_ARRAY);
													 glVertexPointer(2, GL_FLOAT, 0, &vertexData[0]);
													 glColorPointer(4, GL_FLOAT, 0, &colorData[0]);
													 glDrawArrays(GL_TRIANGLES, 0, dc.mTriangles.size());
													 glDisableClientState(GL_VERTEX_ARRAY);
													 glDisableClientState(GL_COLOR_ARRAY);
													 break;
		}

		case DrawCommand::Type::SET_BLEND_MODE:
		{
												  SetBlendModeDrawCommand& dc = drawCommand->as<SetBlendModeDrawCommand>();
												  mInternal.setBlendMode(dc.mBlendMode);
												  break;
		}

		case DrawCommand::Type::SET_SAMPLING_MODE:
		{
													 SetSamplingModeDrawCommand& dc = drawCommand->as<SetSamplingModeDrawCommand>();
													 mInternal.mCurrentSamplingMode = dc.mSamplingMode;
													 break;
		}

		case DrawCommand::Type::SET_WRAP_MODE:
		{
												 SetWrapModeDrawCommand& dc = drawCommand->as<SetWrapModeDrawCommand>();
												 mInternal.mCurrentWrapMode = dc.mWrapMode;
												 break;
		}

		case DrawCommand::Type::PRINT_TEXT:
		{
											  PrintTextDrawCommand& dc = drawCommand->as<PrintTextDrawCommand>();
											  mInternal.printText(*dc.mFont, dc.mText, dc.mRect, dc.mPrintOptions);
											  break;
		}

		case DrawCommand::Type::PRINT_TEXT_W:
		{
												PrintTextWDrawCommand& dc = drawCommand->as<PrintTextWDrawCommand>();
												mInternal.printText(*dc.mFont, dc.mText, dc.mRect, dc.mPrintOptions);
												break;
		}

		case DrawCommand::Type::PUSH_SCISSOR:
		{
												PushScissorDrawCommand& dc = drawCommand->as<PushScissorDrawCommand>();
												Recti scissorRect = dc.mRect;
												if (mInternal.mScissorStack.empty())
													glEnable(GL_SCISSOR_TEST);
												else
													scissorRect.intersect(mInternal.mScissorStack.back());
												mInternal.mScissorStack.push_back(scissorRect);
												mInternal.applyScissor(scissorRect);
												break;
		}

		case DrawCommand::Type::POP_SCISSOR:
		{
											   mInternal.mScissorStack.pop_back();
											   if (mInternal.mScissorStack.empty())
											   {
												   glDisable(GL_SCISSOR_TEST);
											   }
											   else
											   {
												   const Recti scissorRect = mInternal.mScissorStack.back();
												   mInternal.applyScissor(scissorRect);
											   }
											   break;
		}
		}

	}
}

void FixedFunctionDrawer::presentScreen()
{
	psglSwap();
}

#endif
