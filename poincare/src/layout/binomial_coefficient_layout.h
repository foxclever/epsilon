#ifndef POINCARE_BINOMIAL_COEFFICIENT_LAYOUT_H
#define POINCARE_BINOMIAL_COEFFICIENT_LAYOUT_H

#include <poincare/static_layout_hierarchy.h>
#include <poincare/layout_engine.h>

namespace Poincare {

class BinomialCoefficientLayout : public StaticLayoutHierarchy<2> {
public:
  using StaticLayoutHierarchy::StaticLayoutHierarchy;
  ExpressionLayout * clone() const override;
  void backspaceAtCursor(ExpressionLayoutCursor * cursor) override;
  bool moveLeft(ExpressionLayoutCursor * cursor) override;
  bool moveRight(ExpressionLayoutCursor * cursor) override;
  bool moveUp(ExpressionLayoutCursor * cursor, ExpressionLayout * previousLayout = nullptr, ExpressionLayout * previousPreviousLayout = nullptr) override;
  bool moveDown(ExpressionLayoutCursor * cursor, ExpressionLayout * previousLayout = nullptr, ExpressionLayout * previousPreviousLayout = nullptr) override;
  int writeTextInBuffer(char * buffer, int bufferSize) const override {
    return LayoutEngine::writePrefixExpressionLayoutTextInBuffer(this, buffer, bufferSize, "binomial");
  }
protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDSize computeSize() override;
  void computeBaseline() override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
private:
  ExpressionLayout * kLayout();
  ExpressionLayout * nLayout();
};

}

#endif
