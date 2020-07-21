#pragma once

// If you want to use UniqueDiskPtr and DiskPos function,
// You should include this header not include UniqueDiskPtr.hpp and DiskPos.hpp alone.
// Because some function of UniqueDiskPtr and DiskPos need code here.
#include "ByteConverter_basic.hpp"
#include "DiskPos.hpp"
#include "ByteConverter_DiskPos.hpp"
#include "DiskPtr.hpp"
#include "ByteConverter_DiskPtr.hpp"
#include "DiskRef.hpp"
#include "ByteConverter_DiskRef.hpp"