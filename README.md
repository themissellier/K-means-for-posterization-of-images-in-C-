## 🧠 K-Means Image Posterization in C

This project implements an image posterization algorithm using the K-Means clustering method.
It reduces the number of colors in a PPM image (.ppm format) to create a simplified, stylized version while preserving the main visual structure and color tones.

## 🧩 Overview

The program reads a PPM image, analyzes all its pixels, and groups them into clusters based on color similarity.
Each cluster’s pixels are replaced with the average (mean) color of that cluster — resulting in a smooth posterized effect.

The user specifies the number of color means (k) at runtime, allowing different levels of simplification.

## ⚙️ How It Works

Read Input Image – Loads pixel data from a .ppm file (image.ppm here) into a custom Surface class.

Initialize Means – Generates k random RGB centroids.

Assign Pixels – Each pixel is assigned to the nearest mean (color cluster).

Recalculate Means – Computes new average colors for each cluster.

Iterate – Repeats until all centroids are stable (ideal barycenters).

Write Output – Creates a new draw.ppm image using the updated color values.

## 🧰 Technologies

C++ (Standard Library, Vectors, Streams)

K-Means Algorithm for clustering colors

PPM Image Format (Portable Pixmap)

## 📸 Example with 4 means
<p align="center"> <img src="image.png" alt="Original Image" width="45%"> <img src="draw.png" alt="Posterized Image" width="45%"> </p>
