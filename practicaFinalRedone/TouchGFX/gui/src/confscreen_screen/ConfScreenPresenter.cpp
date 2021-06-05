#include <gui/confscreen_screen/ConfScreenView.hpp>
#include <gui/confscreen_screen/ConfScreenPresenter.hpp>

ConfScreenPresenter::ConfScreenPresenter(ConfScreenView& v)
    : view(v)
{

}

void ConfScreenPresenter::activate()
{
	this->SetFPSValue(this->model->GetFPS());
	this->SetMaxTempValue(this->model->GetMaxTemp());
	this->SetMinTempValue(this->model->GetMinTemp());
}

void ConfScreenPresenter::deactivate()
{

}

/***************************************************************/
/**
 * @brief Método que actualiza el valor en la vista de los fps de la cámara
 *
 * @param fps: valor de los frame por segundo configurados en la cámara AMG8833
 */
void ConfScreenPresenter::ChangeFPSValue(bool operation)
{
	this->model->ChangeFPSValue(operation);
	this->SetFPSValue(this->model->GetFPS());
}

/***************************************************************/
void ConfScreenPresenter::ChangeMaxTempValue(bool operation)
{
	this->model->ChangeMaxTempValue(operation);
	this->SetMaxTempValue(this->model->GetMaxTemp());
}

/***************************************************************/
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
