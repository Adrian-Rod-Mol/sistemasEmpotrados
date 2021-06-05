#include <gui/mainscreen_screen/MainScreenView.hpp>


uint8_t pxWidth 	= 240;
uint8_t pxHeigth 	= 240;

uint16_t *psram = (uint16_t*) (0xd0000000 + 240 * 320 * 2 * 2); // Address after two 16 bit frame buffers


MainScreenView::MainScreenView()
{

}

void MainScreenView::setupScreen()
{
    MainScreenViewBase::setupScreen();
    Bitmap::setCache(psram, 320 * 1024, 1); 						// 320Kb cache
	BitmapId bmpId = Bitmap::dynamicBitmapCreate(pxWidth, 						// Width
												 pxHeigth, 						// Height
												 Bitmap::L8, 					// Bitmap Color frame format
												 Bitmap::CLUT_FORMAT_L8_RGB888 	// Palette color format
	);

	this->camImage.setBitmap(Bitmap(bmpId));
	if (bmpId == BITMAP_INVALID)
	{
		touchgfx_printf("Unable to create dynamic bitmap\n");
	}
	else
	{
		uint8_t* data = Bitmap::dynamicBitmapGetAddress(bmpId);

		uint8_t* pixel = data;
		//make colored rows
		for (int y = 5; y<245; y++)
		{
			for (int x = 0; x<240; x++) {
				pixel[(y - 5)*pxWidth + x] = y;
			}

		}

		uint32_t byteSize = 240*240;
		byteSize = ((byteSize + 3) & ~3);

		//Palette starts four bytes after the pixels
		uint8_t* pal = (data + byteSize + 4);

		uint8_t R = 0;
		uint8_t G = 0;
		uint8_t B = 0xFF;

		//Make palette with 256 colors from green to red to green
		for (int i = 0; i<256; i++)
		{
			if ((i > 0) && (i < 64)) {
				G = G + 4;
			} else if ((i >= 64) && (i < 127)) {
				B = B - 4;
			} else if ((i > 127) && (i < 191)) {
				R = R + 4;
			}  else if ((i >= 191) && (i < 255)) {
				G = G - 4;
			}

			//BGR
			pal[i*3 + 0] = B;
			pal[i*3 + 1] = G;
			pal[i*3 + 2] = R;
		}

	}
}

void MainScreenView::tearDownScreen()
{
	Bitmap::dynamicBitmapDelete(this->camImage.getBitmap());
    MainScreenViewBase::tearDownScreen();
}

/***************************************************************/
/**
 * @brief Método que cambia la visibilidad del bitmap
 */
void MainScreenView::ToggleCamBitmap()
{
	this->presenter->HandleBitmapVisibility(this->camImage.isVisible());
}

/***************************************************************/
/**
 * @brief Muestra u oculta el bitmap
 */
void MainScreenView::HandleCamBitmap()
{
	camImage.invalidate();
}

/***************************************************************/
/**
 * @brief Muestra u oculta el bitmap
 */
void MainScreenView::HandleCamState(bool state)
{
	this->playButton.forceState(state);
	this->camImage.setVisible(state);

	playButton.invalidate();
	camImage.invalidate();

}

/***************************************************************/
/**
 * @brief Muestra u oculta el bitmap
 */
uint8_t* MainScreenView::GetImageBitmap()
{
	return Bitmap::dynamicBitmapGetAddress(this->camImage.getBitmap());
}
