extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <poincare/matrix.h>
#include <poincare/float.h>
#include "layout/matrix_layout.h"
#include <math.h>
#include <string.h>

Matrix::Matrix(MatrixData * matrixData) :
  m_matrixData(matrixData)
{
}

Matrix::~Matrix() {
  delete m_matrixData;
}

Matrix::Matrix(Expression ** newOperands, int numberOfOperands, int numberOfColumns, int numberOfRows, bool cloneOperands)
{
  m_matrixData = new MatrixData(newOperands, numberOfOperands, numberOfColumns, numberOfRows, cloneOperands);
}

Integer * Matrix::defaultExpression() {
  static Integer * defaultExpression = new Integer(0);
  return defaultExpression;
}

int Matrix::numberOfOperands() const {
  return m_matrixData->numberOfRows() * m_matrixData->numberOfColumns();
}

const Expression * Matrix::operand(int i) const {
  assert(i >= 0);
  assert(i < numberOfOperands());
  return m_matrixData->operands()[i];
}

Expression * Matrix::clone() const {
  return this->cloneWithDifferentOperands(m_matrixData->operands(), numberOfOperands(), true);
}

ExpressionLayout * Matrix::createLayout() const {
  ExpressionLayout ** childrenLayouts = (ExpressionLayout **)malloc(numberOfOperands()*sizeof(ExpressionLayout *));
  for (int i = 0; i < numberOfOperands(); i++) {
    childrenLayouts[i] = operand(i)->createLayout();
  }
  return new MatrixLayout(childrenLayouts, numberOfRows(), numberOfColumns());
}

float Matrix::approximate(Context& context) const {
  return NAN;
}

Expression * Matrix::evaluate(Context& context) const {
  Expression * operands[numberOfOperands()];
  for (int i = 0; i < numberOfOperands(); i++) {
    operands[i] = new Float(operand(i)->approximate(context));
  }
  return new Matrix(new MatrixData(operands, numberOfOperands(), numberOfColumns(), numberOfRows(), false));
}

Expression::Type Matrix::type() const {
  return Type::Matrix;
}

Expression * Matrix::cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  return new Matrix(newOperands, numberOfOperands, numberOfColumns(), numberOfRows(), cloneOperands);
}

int Matrix::numberOfRows() const {
  return m_matrixData->numberOfRows();
}

int Matrix::numberOfColumns() const {
  return m_matrixData->numberOfColumns();
}

int Matrix::writeTextInBuffer(char * buffer, int bufferSize) {
  buffer[bufferSize] = 0;
  int currentChar = 0;
  if (currentChar >= bufferSize) {
    return 0;
  }
  buffer[currentChar++] = '[';
  if (currentChar >= bufferSize) {
    return currentChar;
  }
  for (int i = 0; i < m_matrixData->numberOfRows(); i++) {
    buffer[currentChar++] = '[';
    if (currentChar >= bufferSize) {
      return currentChar;
    }
    currentChar += m_matrixData->operands()[i*m_matrixData->numberOfColumns()]->writeTextInBuffer(buffer+currentChar, bufferSize-currentChar);
    if (currentChar >= bufferSize) {
      return currentChar;
    }
    for (int j = 1; j < numberOfColumns(); j++) {
      buffer[currentChar++] = ',';
      if (currentChar >= bufferSize) {
        return currentChar;
      }
      currentChar += m_matrixData->operands()[i*m_matrixData->numberOfColumns()+j]->writeTextInBuffer(buffer+currentChar, bufferSize-currentChar);
      if (currentChar >= bufferSize) {
        return currentChar;
      }
    }
    currentChar = strlen(buffer);
    if (currentChar >= bufferSize) {
      return currentChar;
    }
    buffer[currentChar++] = ']';
    if (currentChar >= bufferSize) {
      return currentChar;
    }
  }
  buffer[currentChar++] = ']';
  if (currentChar >= bufferSize) {
    return currentChar;
  }
  buffer[currentChar] = 0;
  return currentChar;
}