#include <gui/mainscreen_screen/MainScreenView.hpp>
#include <gui/mainscreen_screen/MainScreenPresenter.hpp>

#include <cstring>
#include <math.h>

MainScreenPresenter::MainScreenPresenter(MainScreenView& v)
    : view(v)
{

}

void MainScreenPresenter::activate()
{
	this->model->SetCurrentScreen(1);

	this->SetBitmapVisibility(this->model->GetCamState());
	this->SetTargetState(this->model->GetTargetState());
}

void MainScreenPresenter::deactivate()
{

}

void MainScreenPresenter::SetMainTopBar(float cpuTemp, float sensorTemp, float fps)
{
	this->view.HandleTopBarData(cpuTemp, sensorTemp, fps);
}

void MainScreenPresenter::HandleBitmapVisibility(bool state)
{
	this->model->ChangeBitmapState(state);
	this->SetBitmapVisibility(this->model->GetCamState());
}

void MainScreenPresenter::TakeScreenshot()
{
	this->model->SendScreenshot();
}

void MainScreenPresenter::HandleTargetTemp(bool state)
{
	this->model->ChangeTargetState(state);
	this->SetTargetState(this->model->GetTargetState());
}

void MainScreenPresenter::SetBitmapVisibility(bool state)
{
	this->view.HandleCamState(state);

}

void MainScreenPresenter::SetTargetState(bool state)
{
	this->view.HandleTargetState(state);
}

void MainScreenPresenter::SetTargetValue(float temperature)
{
	this->view.HandleTargetTemp(temperature);
}

void MainScreenPresenter::SetBitmapValues(float *frame)
{
	uint8_t framePx	[64];
	uint8_t *data = this->view.GetImageBitmap();
	uint8_t pxInpx	= 30;					// Número de píxeles de la pantalla en cada pixel de la cámara

	// Se calcula los valores de los píxeles del bitmap
	for (int px = 0; px < 64; px++) {

		framePx[px] = frame[px]*3.1875; 		// 255/80 = 3.1875
	}

	// Se asignan los valores a los píxeles de la pantalla
	for (int pxHgth = 0; pxHgth < 8; pxHgth++) {
		for (int pxWdth = 0; pxWdth < 8; pxWdth++) {
			// Rellena la primera fila de píxeles por cada píxel de alto
			memset((void *) &data[pxHgth * 240 * pxInpx + pxWdth * pxInpx],
					framePx[pxHgth *8 + pxWdth],
					pxInpx);
		}

		// Se va incrementando en 2 el número de filas que se copian, se para en 16 y luego
		// se copian las 14 restantes

		for (int blckRow = 0; blckRow < 4; blckRow++) {
			// Copia la primera fila del pixel en las demás filas
			int index = (pow(2, blckRow) * 240);
			memcpy((void*) &data[pxHgth * 240 * pxInpx + index],
				   (void*) &data[pxHgth * 240 * pxInpx],
				   (pow(2, blckRow) * 240));
		}

		memcpy((void*) &data[pxHgth * 240 * pxInpx + (16 * 240)],
			   (void*) &data[pxHgth * 240 * pxInpx],
			   14 * 240);
	}



	uint32_t byteSize = 240*240;
	byteSize = ((byteSize + 3) & ~3);

	//Palette starts four bytes after the pixels
	uint8_t* pal = (data + byteSize + 4);

	uint8_t incremento	= (((this->model->GetMaxTemp() - this->model->GetMinTemp())*3.1875)/4);
	uint8_t variacion	= 256 / incremento;

	uint8_t firstLimit 	= this->model->GetMinTemp()*3.1875;
	uint8_t secondLimit = (this->model->GetMinTemp()*3.1875 + incremento);
	uint8_t thirdLimit	= (this->model->GetMinTemp()*3.1875 + 2*incremento);
	uint8_t forthLimit	= (this->model->GetMinTemp()*3.1875 + 3*incremento);
	uint8_t fifthLimit 	= this->model->GetMaxTemp()*3.1875;

	uint8_t R = 0xFF;
	uint8_t G = 0;
	uint8_t B = 0;

	//Make palette with 256 colors from green to red to green
	for (int i = 0; i<256; i++) {
		if ((i > firstLimit) && (i < secondLimit)) {
			if ((G + variacion) < 256) {
				G = G + variacion;

			} else {G = 0xFF;}

		} else if ((i > secondLimit) && (i < thirdLimit)) {
			if ((R - variacion) >= 0) {
				R = R - variacion;

			} else {R = 0;}

		} else if ((i > thirdLimit) && (i < forthLimit)) {
			if ((B + variacion) < 256) {
				B = B + variacion;

			} else {B = 0xFF;}

		}  else if ((i > forthLimit) && (i < fifthLimit)) {
			if ((G - variacion) >= 0) {
				G = G - variacion;

			} else {G = 0;}
		}

		pal[i*3 + 0] = R;
		pal[i*3 + 1] = G;
		pal[i*3 + 2] = B;
	}

	this->view.HandleCamBitmap();

}
