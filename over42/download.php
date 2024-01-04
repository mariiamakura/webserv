<?php
// Enable error reporting for debugging
error_reporting(E_ALL);
ini_set('display_errors', 1);

// Validate the 'file' query parameter
if (!isset($_GET['file'])) {
    http_response_code(400); // Bad Request
    exit('File parameter is missing.');
}

// Get the requested file name
$requestedFile = $_GET['file'];

// Validate and sanitize the file name to prevent directory traversal attacks
$allowedChars = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-.';
if (strspn($requestedFile, $allowedChars) !== strlen($requestedFile)) {
    http_response_code(400); // Bad Request
    exit('Invalid file name.');
}

// Path to the 'download' directory (adjust the path as needed)
$downloadPath = '../download/';

// Full path to the requested file
$filePath = $downloadPath . $requestedFile;

// Check if the file exists
if (!file_exists($filePath)) {
    http_response_code(404); // Not Found
    exit('File not found: ' . $requestedFile);
}

// Set headers for file download
header('Content-Description: File Transfer');
header('Content-Type: application/octet-stream');
header('Content-Disposition: attachment; filename="' . basename($filePath) . '"');
header('Expires: 0');
header('Cache-Control: must-revalidate');
header('Pragma: public');
header('Content-Length: ' . filesize($filePath));

// Read the file and output its content
readfile($filePath);
?>
