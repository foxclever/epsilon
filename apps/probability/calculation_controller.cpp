#include "calculation_controller.h"
#include "../constant.h"
#include "../apps_container.h"
#include <assert.h>

namespace Probability {

CalculationController::ContentView::ContentView(Responder * parentResponder,Calculation * calculation) :
  m_lawCurveView(LawCurveView()),
  m_imageTableView(ImageTableView(parentResponder, calculation)),
  m_calculation(calculation)
{
  for (int k = 0; k < k_maxNumberOfEditableFields; k++) {
    m_calculationCell[k].setParentResponder(parentResponder);
    CalculationController * parentController = (CalculationController *)parentResponder;
    m_calculationCell[k].setDelegate(parentController);
  }
}

void CalculationController::ContentView::setLaw(Law * law) {
  m_lawCurveView.setLaw(law);
}

int CalculationController::ContentView::numberOfSubviews() const {
  if ((int)m_calculation->type() == 1) {
    return 8;
  }
  return 6;
}

View * CalculationController::ContentView::subviewAtIndex(int index) {
  assert(index >= 0 && index < 8);
  if (index == 0) {
    return &m_lawCurveView;
  }
  if (index == 1) {
    return &m_imageTableView;
  }
  if (index == 2) {
    if ((int)m_calculation->type() == 0) {
      m_text[0].setText("P(X <= ");
    } else {
      m_text[0].setText("P( ");
    }
    m_text[0].setAlignment(0.5f, 0.5f);
    return &m_text[0];
  }
  if (index == 4) {
    if (m_calculation->type() == Calculation::Type::LeftIntegral) {
      m_text[1].setText(") = ");
    }
    if (m_calculation->type() == Calculation::Type::FiniteIntegral) {
      m_text[1].setText(" <= X <= ");
    }
    if (m_calculation->type() == Calculation::Type::RightIntegral) {
      m_text[1].setText(" <= X ) = ");
    }
    m_text[1].setAlignment(0.5f, 0.5f);
    return &m_text[1];
  }
  if (index == 6) {
    m_text[2].setText(" )= ");
    m_text[2].setAlignment(0.5f, 0.5f);
    return &m_text[2];
  }
  if (index == 3 || index == 5 || index == 7) {
    return &m_calculationCell[(index - 3)/2];
  }
  return nullptr;
}

void CalculationController::ContentView::willDisplayEditableCellAtIndex(int index) {
  char buffer[Constant::FloatBufferSizeInScientificMode];
  Float(m_calculation->parameterAtIndex(index)).convertFloatToText(buffer, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
  m_calculationCell[index].setText(buffer);
}

void CalculationController::ContentView::layoutSubviews() {
  markRectAsDirty(bounds());
  KDCoordinate xCoordinate = 0;
  m_lawCurveView.setFrame(KDRect(0,  ImageTableView::k_imageHeight, bounds().width(), bounds().height() - ImageTableView::k_imageHeight));
  m_imageTableView.setFrame(KDRect(xCoordinate, 0, ImageTableView::k_imageWidth, 3*ImageTableView::k_imageHeight));
  xCoordinate += ImageTableView::k_imageWidth + k_textMargin;
  KDCoordinate numberOfCharacters = 7;
  if ((int)m_calculation->type() > 0) {
    numberOfCharacters = 3;
  }
  m_text[0].setFrame(KDRect(xCoordinate, 0, numberOfCharacters*k_charWidth, ImageTableView::k_imageHeight));
  xCoordinate += numberOfCharacters*k_charWidth + k_textMargin;
  m_calculationCell[0].setFrame(KDRect(xCoordinate, 0, k_textFieldWidth, ImageTableView::k_imageHeight));
  xCoordinate += k_textFieldWidth + k_textMargin;
  if (m_calculation->type() == Calculation::Type::LeftIntegral) {
    numberOfCharacters = 4;
  }
  if (m_calculation->type() == Calculation::Type::FiniteIntegral) {
    numberOfCharacters = 9;
  }
  if (m_calculation->type() == Calculation::Type::RightIntegral) {
    numberOfCharacters = 10;
  }
  m_text[1].setFrame(KDRect(xCoordinate, 0, numberOfCharacters*k_charWidth, ImageTableView::k_imageHeight));
  xCoordinate += numberOfCharacters*k_charWidth + k_textMargin;
  m_calculationCell[1].setFrame(KDRect(xCoordinate, 0, k_textFieldWidth, ImageTableView::k_imageHeight));
  xCoordinate += k_textFieldWidth + k_textMargin;
  numberOfCharacters = 4;
  m_text[2].setFrame(KDRect(xCoordinate, 0, numberOfCharacters*k_charWidth, ImageTableView::k_imageHeight));
  xCoordinate += numberOfCharacters*k_charWidth + k_textMargin;
  m_calculationCell[2].setFrame(KDRect(xCoordinate, 0, k_textFieldWidth, ImageTableView::k_imageHeight));

  for (int k = 0; k < k_maxNumberOfEditableFields; k++) {
    willDisplayEditableCellAtIndex(k);
  }
}


void CalculationController::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), KDColorWhite);
}

LawCurveView * CalculationController::ContentView::lawCurveView() {
  return &m_lawCurveView;
}

ImageTableView * CalculationController::ContentView::imageTableView() {
  return &m_imageTableView;
}

EditableTextCell * CalculationController::ContentView::calculationCellAtIndex(int index) {
  return &m_calculationCell[index];
}

CalculationController::CalculationController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_contentView(ContentView(this, &m_calculation)),
  m_highlightedSubviewIndex(1),
  m_calculation(Calculation())
{
}

View * CalculationController::view() {
  return &m_contentView;
}

const char * CalculationController::title() const {
  return "Calculer les probabilites";
}

void CalculationController::setLaw(Law * law) {
  m_contentView.setLaw(law);
  m_calculation.setLaw(law);
}

Calculation * CalculationController::calculation() {
  return &m_calculation;
}

bool CalculationController::handleEvent(Ion::Events::Event event) {

  if ((event == Ion::Events::Left && m_highlightedSubviewIndex > 0) || (event == Ion::Events::Right && m_highlightedSubviewIndex < ContentView::k_maxNumberOfEditableFields - 1)) {
    if (m_highlightedSubviewIndex == 0) {
      m_contentView.imageTableView()->select(false);
      m_contentView.imageTableView()->setHighlight(false);
      m_contentView.layoutSubviews();
    } else {
      EditableTextCell * calculCell = m_contentView.calculationCellAtIndex(m_highlightedSubviewIndex-1);
      calculCell->setHighlighted(false);
    }
    m_highlightedSubviewIndex = event == Ion::Events::Left ? m_highlightedSubviewIndex - 1 : m_highlightedSubviewIndex + 1;
    if (m_highlightedSubviewIndex > 0) {
      EditableTextCell * newCalculCell = m_contentView.calculationCellAtIndex(m_highlightedSubviewIndex-1);
      newCalculCell->setHighlighted(true);
      app()->setFirstResponder(newCalculCell);
    } else {
      m_contentView.imageTableView()->setHighlight(true);
      app()->setFirstResponder(this);
    }
    return true;
  }
  if (event == Ion::Events::OK && m_highlightedSubviewIndex == 0) {
    m_contentView.imageTableView()->select(true);
    app()->setFirstResponder(m_contentView.imageTableView());
    return true;
  }
  return false;
}


bool CalculationController::textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) {
  App * myApp = (App *)app();
  return myApp->textFieldDidReceiveEvent(textField, event);
}

bool CalculationController::textFieldDidFinishEditing(TextField * textField, const char * text) {
  AppsContainer * appsContainer = (AppsContainer *)app()->container();
  Context * globalContext = appsContainer->context();
  float floatBody = Expression::parse(text)->approximate(*globalContext);
  m_calculation.setParameterAtIndex(floatBody, m_highlightedSubviewIndex-1);
  for (int k = 0; k < ContentView::k_maxNumberOfEditableFields; k++) {
    m_contentView.willDisplayEditableCellAtIndex(k);
  }
  return true;
}

void CalculationController::didBecomeFirstResponder() {
  m_contentView.layoutSubviews();
  for (int subviewIndex = 0; subviewIndex < 2; subviewIndex++) {
    EditableTextCell * calculCell = m_contentView.calculationCellAtIndex(subviewIndex);
    calculCell->setHighlighted(false);
  }
  if (m_highlightedSubviewIndex > 0) {
    m_contentView.imageTableView()->setHighlight(false);
    EditableTextCell * calculCell = m_contentView.calculationCellAtIndex(m_highlightedSubviewIndex-1);
    calculCell->setHighlighted(true);
    app()->setFirstResponder(calculCell);
  } else {
    m_contentView.imageTableView()->setHighlight(true);
  }
  m_contentView.lawCurveView()->reload();
}

void CalculationController::selectSubview(int subviewIndex) {
  m_highlightedSubviewIndex = subviewIndex;
}

}