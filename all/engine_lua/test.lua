print ("Content-Type:text/html\r\n\r\n")


--res = _G.mysql.con:execute"DROP TABLE people"
--res = assert (mysql.con:execute[[CREATE TABLE people(name  varchar(50),email varchar(50))]])
-- add a few elements
--[[list = {
  { name="Jose das Couves", email="jose@couves.com", },
  { name="Manoel Joaquim", email="manoel.joaquim@cafundo.com", },
  { name="Maria das Dores", email="maria@dores.com", },
}
for i, p in pairs (list) do
  res = assert (mysql.con:execute(string.format("INSERT INTO people VALUES ('%s', '%s')", p.name, p.email)))
end
]]
-- retrieve a cursor
cur = assert (mysql.query("SELECT name, email from people where name = 'dd'"))
-- print all rows, the rows will be indexed by field names
row = cur:fetch ({}, "a")
while row do
  print(string.format("Name: %s, E-mail: %s<br/>\r\n", row.name, row.email))
  -- reusing the table of results
  row = cur:fetch (row, "a")
end
-- close everything
cur:close()

