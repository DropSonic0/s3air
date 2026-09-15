/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#pragma once

#ifdef RMX_WITH_OPENGL_SUPPORT

#include "oxygen/drawing/DrawerInterface.h"

namespace fixedfunctiondrawer
{
	struct Internal;
}

class FixedFunctionDrawer : public DrawerInterface
{
public:
	FixedFunctionDrawer();
	virtual ~FixedFunctionDrawer();

	virtual Drawer::Type getType() override { return Drawer::Type::OPENGL; }
	virtual bool wasSetupSuccessful() override;

	virtual void createTexture(DrawerTexture& outTexture) override;
	virtual void refreshTexture(DrawerTexture& texture) override;

	virtual void setupRenderWindow(SDL_Window* window) override;

	virtual void performRendering(const DrawCollection& drawCollection) override;
	virtual void presentScreen() override;

private:
	fixedfunctiondrawer::Internal& mInternal;
};

#endif
