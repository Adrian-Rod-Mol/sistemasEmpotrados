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

void MainScreenPresenter::SetBitmapValues()
{
	this->view.HandleCamBitmap();

}
