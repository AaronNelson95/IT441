/* JavaScript Codes for Travel Website
	Aaron Nelson
	Last Updated: 3-25-17 */

/* This Function formats lastModified so that it does not include the time, only the date */
function writeDateModified() {
	var modDate = new Date(Date.parse(document.lastModified));

	if (modDate != 0 && modDate != "Invalid Date") {
		var ndate = modDate.getDate();
		var month = modDate.getMonth();
		var year = modDate.getYear();
	if (year < 1000) year = year + 1900;
    
    return((month + 1) + "/" + ndate + "/" + year+ "  ");
  }
}
