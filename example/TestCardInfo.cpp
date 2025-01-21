#include "./TestCardInfo.h"

TestCardInfo::TestCardInfo(
  TestCardInfo::CardType type,
  const std::string& imageName
):
  CardInfo(imageName),
  type(type)
{
}
