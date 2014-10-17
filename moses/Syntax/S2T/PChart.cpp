#include "PChart.h"

namespace Moses
{
namespace Syntax
{
namespace S2T
{

PChart::PChart(std::size_t width, bool maintainCompressedChart)
{
  cells.resize(width);
  for (std::size_t i = 0; i < width; ++i) {
    cells[i].resize(width);
  }
  if (maintainCompressedChart) {
    m_compressed = new CompressedChart(width);
    // TODO Initialize
  }
}

PChart::~PChart()
{
  delete m_compressed;
}

}  // namespace S2T
}  // namespace Syntax
}  // namespace Moses
