# Define the variables for GLFW3
RELEASE_URL="https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.zip"
DOWNLOAD_DIR="external"
GLFW_DIR="${DOWNLOAD_DIR}/glfw3"
ZIP_FILE="${DOWNLOAD_DIR}/glfw3.zip"

# Remove the directory if it exists
rm -rf "${DOWNLOAD_DIR}"

# Create the external directory
mkdir -p "${DOWNLOAD_DIR}"

# Download the specific release zip file
echo "Downloading GLFW 3.4 source code..."
curl -L -o "${ZIP_FILE}" "${RELEASE_URL}"

# Check if the download was successful
if [ ! -f "${ZIP_FILE}" ]; then
  echo "Failed to download GLFW source code."
  exit 1
fi

# Extract the zip file into the glfw3 directory
echo "Extracting GLFW source code..."
unzip -o "${ZIP_FILE}" -d "${GLFW_DIR}"

# Find the actual extracted folder (it might be named glfw-3.4)
EXTRACTED_DIR=$(find "${GLFW_DIR}" -mindepth 1 -maxdepth 1 -type d)

# Move the contents to the desired glfw3 directory (if necessary)
if [ -d "${EXTRACTED_DIR}" ] && [ "${EXTRACTED_DIR}" != "${GLFW_DIR}" ]; then
  mv "${EXTRACTED_DIR}"/* "${GLFW_DIR}"
  rmdir "${EXTRACTED_DIR}"
fi

# Remove the downloaded zip file
echo "Cleaning up..."
rm -f "${ZIP_FILE}"

# Change to the GLFW directory
cd "${GLFW_DIR}" || exit

# Run CMake and Make commands
echo "Running cmake and make..."
cmake -S . -B build -DBUILD_SHARED_LIBS=ON
cd build
make

echo "GLFW 3.4 has been successfully built in ${GLFW_DIR}/build."

#return to project's root
cd ../../../

# Define the variables for GLM
RELEASE_URL="https://github.com/g-truc/glm/archive/refs/tags/1.0.1.zip"
DOWNLOAD_DIR="external"
GLM_DIR="${DOWNLOAD_DIR}/glm"
ZIP_FILE="${DOWNLOAD_DIR}/glm.zip"

# Create the external directory if it doesn't exist
mkdir -p "${DOWNLOAD_DIR}"

# Download the specific release zip file
echo "Downloading GLM 1.0.1 source code..."
curl -L -o "${ZIP_FILE}" "${RELEASE_URL}"

# Check if the download was successful
if [ ! -f "${ZIP_FILE}" ]; then
  echo "Failed to download GLM source code."
  exit 1
fi

# Extract the zip file into the glm directory
echo "Extracting GLM source code..."
unzip -o "${ZIP_FILE}" -d "${DOWNLOAD_DIR}"

# Find the actual extracted folder (it might be named glm-1.0.1)
EXTRACTED_DIR=$(find "${DOWNLOAD_DIR}" -mindepth 1 -maxdepth 1 -type d -name "glm-*")

# Move the 'glm' subfolder into the desired glm directory
if [ -d "${EXTRACTED_DIR}/glm" ]; then
  echo "Copying GLM headers to ${GLM_DIR}..."
  mkdir -p "${GLM_DIR}"
  cp -r "${EXTRACTED_DIR}/glm"/* "${GLM_DIR}"
else
  echo "Could not find the 'glm' subdirectory in the extracted files."
  exit 1
fi

# Remove the extracted directory and zip file
echo "Cleaning up..."
rm -rf "${EXTRACTED_DIR}"
rm -f "${ZIP_FILE}"

echo "GLM headers have been successfully copied to ${GLM_DIR}."

# Define variables for TinyObjLoader
HEADER_URL="https://raw.githubusercontent.com/tinyobjloader/tinyobjloader/master/tiny_obj_loader.h"
DOWNLOAD_DIR="external/tinyobjloader"
HEADER_FILE="${DOWNLOAD_DIR}/tiny_obj_loader.h"

# Create the download directory if it doesn't exist
mkdir -p "${DOWNLOAD_DIR}"

# Download the tiny_obj_loader.h file
echo "Downloading tiny_obj_loader.h..."
curl -L -o "${HEADER_FILE}" "${HEADER_URL}"

# Check if the download was successful
if [ ! -f "${HEADER_FILE}" ]; then
  echo "Failed to download tiny_obj_loader.h."
  exit 1
fi

echo "tiny_obj_loader.h has been successfully downloaded to ${HEADER_FILE}."


mkdir -p build
cd build
cmake -S .. -B .
make & make Shaders
cd ..