#include <gui/containers/TopBar.hpp>

TopBar::TopBar()
{

}

void TopBar::initialize()
{
    TopBarBase::initialize();
}

/***************************************************************/
/**
 * @brief Actualiza el valor de la temperatura del procesador en la top bar
 *
 * @param temperature: temperatura actual del procesador
 */
void TopBar::SetCPUTemp(float temperature)
{
	Unicode::snprintfFloat(this->cpuTempBuffer, sizeof(this->cpuTempBuffer), "%2.1f", temperature);

}

/***************************************************************/
/**
 * @brief Actualiza el valor de la temperatura del sensor de la cámara
 *
 * @param temperature: temperatura actual del sensor de la cámara
 */
void TopBar::SetSensorTemp(float temperature)
{
	Unicode::snprintfFloat(this->sensorTempBuffer, sizeof(this->sensorTempBuffer), "%2.1f", temperature);

}

/***************************************************************/
/**
 * @brief Actualiza el valor de los frame por segundo a los que funciona la pantalla
 *
 * @param temperature: frame por segundo del modelo
 */
void TopBar::SetScreenFPS(float fps)
{
	Unicode::snprintfFloat(this->fpsCountBuffer, sizeof(this->fpsCountBuffer), "%2.1f", fps);

}
