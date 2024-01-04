<?php
//header("Access-Control-Allow-Origin: *");
header("Content-Type: application/json");

$folderPath = '../download';

try {
    // Check if the directory exists
    if (!is_dir($folderPath)) {
        throw new Exception('Directory not found');
    }

    // Get the list of files in the folder
    $fileList = scandir($folderPath);

    // Remove "." and ".." from the file list
    $fileList = array_diff($fileList, array('.', '..'));

    // Convert the array to JSON format
    $jsonResponse = json_encode(array_values($fileList));

    // Check if JSON encoding was successful
    if ($jsonResponse === false) {
        throw new Exception('Error encoding JSON');
    }

    // Output the JSON response
    echo $jsonResponse;

} catch (Exception $e) {
    // Handle exceptions and output an error JSON response
    http_response_code(500);
    echo json_encode(['error' => $e->getMessage()]);
}
?>
