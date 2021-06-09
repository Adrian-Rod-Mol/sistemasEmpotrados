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
	// Se usa para conocer el valor de la pantalla en la que se encuentra el programa
	this->model->SetCurrentScreen(1);

	this->SetBitmapVisibility(this->model->GetCamState());

	// Si la cámara está activa, activa también el puntero
	if (this->model->GetCamState()) {
		this->SetTargetState(this->model->GetTargetState());
	}


}

void MainScreenPresenter::deactivate()
{

}

/***************************************************************/
/**
 * @brief Método que envía los valores del top bar a la vista
 *
 * @param cpuTemp: 		valor de la temperatura del procesador
 * @param sensorTemp: 	valor de la temperatura del sensor de la cámara
 * @param fps: 			valor de los frame por segundo de la pantalla
 */
void MainScreenPresenter::SetMainTopBar(float cpuTemp, float sensorTemp, float fps)
{
	this->view.HandleTopBarData(cpuTemp, sensorTemp, fps);
}

/***************************************************************/
/**
 * @brief Cambia el estado de la variable de control del bitmap y actualiza la vista
 *
 * @param state - true: variable de estado a false, false: variable a true
 */
void MainScreenPresenter::HandleBitmapVisibility(bool state)
{
	this->model->ChangeBitmapState(state);
	this->SetBitmapVisibility(this->model->GetCamState());
}

/***************************************************************/
/**
 * @brief Indica al modelo que envíe una captura de la imagen de la cámara si es posible
 *
 */
void MainScreenPresenter::TakeScreenshot()
{
	this->model->SendScreenshot();
}

/***************************************************************/
/**
 * @brief Cambia el estado de la variable de control del bitmap y actualiza la vista
 *
 * @param state - true: variable de estado a false, false: variable a true
 */
void MainScreenPresenter::HandleTargetTemp(bool state)
{
	this->model->ChangeTargetState(state);
	this->SetTargetState(this->model->GetTargetState());
}

/***************************************************************/
/**
 * @brief Método que actualiza el estado del bitmap en la vista
 *
 * @param state - true: muestra el bitmap, false: oculta el bitmap
 */
void MainScreenPresenter::SetBitmapVisibility(bool state)
{
	this->view.HandleCamState(state);

}

/***************************************************************/
/**
 * @brief Método que actualiza el estado del puntero en la vista
 *
 * @param state - true: muestra el puntero, false: oculta el puntero
 */
void MainScreenPresenter::SetTargetState(bool state)
{
	this->view.HandleTargetState(state);
}

/***************************************************************/
/**
 * @brief Método que actualiza la temperatura media del bitmap en la vista
 *
 * @param temperature: temperatura media de los cuatro pixel centrales del bitmap
 */
void MainScreenPresenter::SetTargetValue(float temperature)
{
	this->view.HandleTargetTemp(temperature);
}

/***************************************************************/
/**
 * @brief Método que genera la imagen a partir del frame de la cámara en la
 * 	      dirección de memoria del bitmap y genera la paleta de colores a
 * 	      partir de los parámetros establecidos en la configuración
 *
 * @param frame: puntero al frame de la cámara
 */
void MainScreenPresenter::SetBitmapValues(float *frame)
{
	uint8_t framePx	[64];							// Frame de la cámara mapeado
	uint8_t *data = this->view.GetImageBitmap();	// Dirección de memoria del bitmap
	uint8_t pxInpx	= 30;							// Número de píxeles de la pantalla en cada pixel de la cámara

	/* Frame */
	/***************************************************************/
	// Se mapean los valores de los píxeles del bitmap
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

		// Se va incrementando en potencias de dos (1, 2, 4, 8) las filas que se copian. Debido
		// a que el pixel de la cámara equivale a 30 px y la siguiente potencia de 2 es 32,
		// se rellenan los últimos 14 valores (8 flias copiadas + 8 nuevas = 16  -> 30 - 16 = 14)
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


	/* Paleta */
	/***************************************************************/
	uint32_t byteSize = 240*240;			// Número de píxeles en la imagen
	byteSize = ((byteSize + 3) & ~3);

	//Palette starts four bytes after the pixels
	uint8_t* pal = (data + byteSize + 4);	// Dirección de la paleta de colores

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

	// Asigna los valores a la paleta según los parámetros de saturación que se
	// han configurado
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

	this->view.HandleCamBitmap();		// Renderiza el bitmap

}
