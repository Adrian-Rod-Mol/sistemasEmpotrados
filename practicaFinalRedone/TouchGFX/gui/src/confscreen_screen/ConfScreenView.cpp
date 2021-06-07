#include <gui/confscreen_screen/ConfScreenView.hpp>

ConfScreenView::ConfScreenView()
{

}

void ConfScreenView::setupScreen()
{
    ConfScreenViewBase::setupScreen();
}

void ConfScreenView::tearDownScreen()
{
    ConfScreenViewBase::tearDownScreen();
}

/* Botones */
/***************************************************************/
/**
 * @brief Método CallBack para aumentar el valor de los FPS
 */
void ConfScreenView::PlusFPS()
{
	this->presenter->ChangeFPSValue(true);
}

/**
 * @brief Método CallBack para disminuir el valor de los FPS
 */
void ConfScreenView::MinusFPS()
{
	this->presenter->ChangeFPSValue(false);
}

/***************************************************************/
/**
 * @brief Método CallBack para aumentar el valor de la
 * 		  máxima temperatura
 */
void ConfScreenView::PlusMaxTemp()
{
	this->presenter->ChangeMaxTempValue(true);
}

/**
 * @brief Método CallBack para dismiuir el valor de la
 * 		  máxima temperatura
 */
void ConfScreenView::MinusMaxTemp()
{
	this->presenter->ChangeMaxTempValue(false);
}

/***************************************************************/
/**
 * @brief Método CallBack para aumentar el valor de la
 * 		  mínima temperatura
 */
void ConfScreenView::PlusMinTemp()
{
	this->presenter->ChangeMinTempValue(true);
}

/**
 * @brief Método CallBack para dismiuir el valor de la
 * 		  mínima temperatura
 */
void ConfScreenView::MinusMinTemp()
{
	this->presenter->ChangeMinTempValue(false);
}


/* Top Bar */
/***************************************************************/
/**
 * @brief Método que actualiza los valores de la top bar
 *
 * @param cpuTemp: 		valor de la temperatura del procesador
 * @param sensorTemp: 	valor de la temperatura del sensor de la cámara
 * @param fps: 			valor de los frame por segundo de la pantalla
 */
void ConfScreenView::HandleTopBarData(float cpuTemp, float sensorTemp, float fps)
{
	this->dataBarConf.SetCPUTemp(cpuTemp);
	this->dataBarConf.SetSensorTemp(sensorTemp);
	this->dataBarConf.SetScreenFPS(fps);

	this->dataBarConf.invalidate();
}


/* Métodos para actualizar valores en pantalla */
/***************************************************************/
/**
 * @brief Método que actualiza el valor del cuadro de texto
 * 		  que muestra los fps de la cámara
 *
 * @param fps: valor de los frame por segundo configurados en la cámara AMG8833
 */
void ConfScreenView::UpdateFPS(uint8_t fps)
{
	Unicode::snprintf(this->fpsConfBuffer, sizeof(this->fpsConfBuffer), "%2d", fps);
	this->fpsConf.invalidate();
}

/***************************************************************/
/**
 * @brief Método que actualiza el valor del cuadro de texto
 * 		  que muestra la temperatura máxima del bitmap
 *
 * @param maxTemp: valor de la temperatura máxima del bitmap del frame de la cámara
 */
void ConfScreenView::UpdateMaxTemp(uint8_t maxTemp)
{
	Unicode::snprintf(this->maxTempConfBuffer, sizeof(this->maxTempConfBuffer), "%2d", maxTemp);
	this->maxTempConf.resizeToCurrentText();
	this->maxTempConf.invalidate();
}

/***************************************************************/
/**
 * @brief Método que actualiza el valor del cuadro de texto
 * 		  que muestra la temperatura mínima del bitmap
 *
 * @param maxTemp: valor de la temperatura mínima del bitmap del frame de la cámara
 */
void ConfScreenView::UpdateMinTemp(uint8_t minTemp)
{
	Unicode::snprintf(this->minTempConfBuffer, sizeof(this->minTempConfBuffer), "%2d", minTemp);
	this->minTempConf.resizeToCurrentText();
	this->minTempConf.invalidate();
}
