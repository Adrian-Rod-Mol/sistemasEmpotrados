#include <gui/mainscreen_screen/MainScreenView.hpp>
#include <gui/mainscreen_screen/MainScreenPresenter.hpp>

MainScreenPresenter::MainScreenPresenter(MainScreenView& v)
    : view(v)
{

}

void MainScreenPresenter::activate()
{
	this->SetBitmapVisibility(this->model->GetCamState());
	this->SetBitmapValues();
}

void MainScreenPresenter::deactivate()
{

}

void MainScreenPresenter::HandleBitmapVisibility(bool state)
{
	this->model->ChangeBitmapState(state);
	this->SetBitmapVisibility(this->model->GetCamState());
}

void MainScreenPresenter::SetBitmapVisibility(bool state)
{
	this->view.HandleCamState(state);

}

void MainScreenPresenter::SetBitmapValues(float *frame)
{
	uint8_t framePx	[64];
	uint8_t *data = this->view.GetImageBitmap();
	uint8_t pxInpx	= 30;					// Número de píxeles de la pantalla en cada pixel de la cámara

	// Se calcula los valores de los píxeles del bitmap
	for (int px = 0; px < 64; px++) {

		framePx[px] = frame[px]*0.3137; 		// 80/255 = 0.3137
	}

	// Se asignan los valores a los píxeles de la pantalla
	for (int pxHgth = 0; pxHgth < 8; pxHgth++) {
		for (int pxWdth = 0; pxWdth < 8; pxWdth++) {
			// Rellena la primera fila de píxeles por cada píxel de alto
			memset(&data[pxHgth * 240 * pxInpx + pxWdth * pxInpx],
					framePx[pxHgth * pxWdth + pxWdth],
					pxInpx);
		}

		// Copia la memoria de manera exponencial (primero la 1ª fila, luego las dos primeras,
		// luego las cuatro primeras...) hasta llegara 16 y luego copia las 30 - 16 = 14 filas restantes
		for (int blckRow = 0; blckRow < 4; blckRow++) {
			// Copia la primera fila del pixel en las demás filas
			memcpy(&data[pxHgth * 240 * pxInpx],
				   &data[pxHgth * 240 * pxInpx + (2^blckRow) * 240],
				   ((2^blckRow) * 240));
		}

		memcpy(&data[pxHgth * 240 * pxInpx],
			   &data[pxHgth * 240 * pxInpx + 16 * 240],
			   14 * 240);

	}

	// Se genera la paleta de colores
	uint32_t byteSize = 240*240;
	byteSize = ((byteSize + 3) & ~3);

	//Palette starts four bytes after the pixels
	uint8_t* pal = (data + byteSize + 4);

	uint8_t incremento	= ((this->model->GetMaxTemp() - this->model->GetMinTemp())/4);
	uint8_t variacion	= 256 / incremento;

	uint8_t firstLimit 	= this->model->GetMinTemp()*0.3137;
	uint8_t secondLimit = (this->model->GetMinTemp() + incremento)*0.3137;
	uint8_t thirdLimit	= (this->model->GetMinTemp() + 2*incremento)*0.3137;
	uint8_t forthLimit	= (this->model->GetMinTemp() + 3*incremento)*0.3137;
	uint8_t fifthLimit 	= this->model->GetMaxTemp()*0.3137;

	uint8_t R = 0;
	uint8_t G = 0;
	uint8_t B = 0xFF;

	//Make palette with 256 colors from green to red to green
	for (int i = 0; i<256; i++) {
		if ((i > 0) && (i < firstLimit)) {
			G = G + variacion;
		} else if ((i >= firstLimit) && (i < secondLimit)) {
			B = B - variacion;
		} else if ((i > secondLimit) && (i < thirdLimit)) {
			R = R + variacion;
		}  else if ((i >= forthLimit) && (i < fifthLimit)) {
			G = G - variacion;
		}

		pal[i*3 + 0] = R;
		pal[i*3 + 1] = G;
		pal[i*3 + 2] = B;
	}
	this->view.HandleCamBitmap();

}
