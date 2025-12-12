target "default" {
  dockerfile = "Dockerfile"
  platforms  = ["linux/amd64", "linux/arm64"]
  pull       = true
  output = [
    "type=local,dest=out",
  ]

  cache-from = [
    "type=gha"
  ]

  cache-to = [
    "type=gha,mode=max"
  ]
}
