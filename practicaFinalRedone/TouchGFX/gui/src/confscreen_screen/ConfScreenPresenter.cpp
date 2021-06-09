#include <gui/confscreen_screen/ConfScreenView.hpp>
#include <gui/confscreen_screen/ConfScreenPresenter.hpp>

ConfScreenPresenter::ConfScreenPresenter(ConfScreenView& v)
    : view(v)
{

}

void ConfScreenPresenter::activate()
{
	// Se usa para conocer el valor de la pantalla en la que se encuentra el programa
	this->model->SetCurrentScreen(2);

	this->SetFPSValue(this->model->GetFPS());
	this->SetMaxTempValue(this->model->GetMaxTemp());
	this->SetMinTempValue(this->model->GetMinTemp());

}

void ConfScreenPresenter::deactivate()
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
void ConfScreenPresenter::SetConfTopBar(float cpuTemp, float sensorTemp, float fps)
{
	this->view.HandleTopBarData(cpuTemp, sensorTemp, fps);
}

/***************************************************************/
/**
 * @brief Método que modifica la variable de control de los fps y la actualiza en la vista
 *
 * @param operation - true: aumenta la variable, false: disminuye la variable
 */
void ConfScreenPresenter::ChangeFPSValue(bool operation)
{
	this->model->ChangeFPSValue(operation);
	this->SetFPSValue(this->model->GetFPS());
}

/***************************************************************/
/**
 * @brief Método que modifica la variable de control de la máxima temperatura de saturación
 *        y la actualiza en la vista
 *
 * @param operation - true: aumenta la variable, false: disminuye la variable
 */
void ConfScreenPresenter::ChangeMaxTempValue(bool operation)
{
	this->model->ChangeMaxTempValue(operation);
	this->SetMaxTempValue(this->model->GetMaxTemp());
}

/***************************************************************/
/**
 * @brief Método que modifica la variable de control de la mínima temperatura de saturación
 *        y la actualiza en la vista
 *
 * @param operation - true: aumenta la variable, false: disminuye la variable
 */
void ConfScreenPresenter::ChangeMinTempValue(bool operation)
{
	this->model->ChangeMinTempValue(operation);
	this->SetMinTempValue(this->model->GetMinTemp());
}

/***************************************************************/
/**
 * @brief Método que actualiza el valor en la vista de los fps de la cámara
 *
 * @param fps: valor de los frame por segundo configurados en la cámara AMG8833
 */
void ConfScreenPresenter::SetFPSValue(uint8_t fps)
{
	this->view.UpdateFPS(fps);
}
/***************************************************************/
/**
 * @brief Método que actualiza el valor en la vista de la máxima temperatura
 *
 * @param maxTemp: valor de la temperatura máxima del bitmap del frame de la cámara
 */
void ConfScreenPresenter::SetMaxTempValue(uint8_t maxTemp)
{
	this->view.UpdateMaxTemp(maxTemp);
}

/***************************************************************/
/**
 * @brief Método que actualiza el valor en la vista de la mínima temperatura
 *
 * @param maxTemp: valor de la temperatura mínima del bitmap del frame de la cámara
 */
void ConfScreenPresenter::SetMinTempValue	(uint8_t minTemp)
{
	this->view.UpdateMinTemp(minTemp);
}
